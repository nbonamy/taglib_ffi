// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:async';
import 'dart:ffi';
import 'dart:isolate';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

import '../src/model.dart';
import '../src/utils.dart';
import '../taglib_ffi_bindings_generated.dart' as taglib;

//
// all of this is a bit awkward and hacky but we want
// to be able to run dart unit-tests without building
// a full macos application (and accessing the dylib through a framework)
//
// but because getArtworkBytes is async and runs through an isolate
// it requires _bindings to be a global variable (at least afaik)
//

taglib.TaglibFfiBindings _bindings =
    taglib.TaglibFfiBindings(loadLibrary('.', false));

class TagLib {
  TagLib({String path = '.', bool test = false}) {
    if (path != '.' || test) {
      _bindings = taglib.TaglibFfiBindings(loadLibrary(path, test));
    }
  }

  Tags getAudioTags(String filename) {
    taglib.Tags nativeTags =
        _bindings.get_audio_tags(filename.toNativeUtf8().cast<Char>());
    Tags tags = Tags.fromNativeTags(nativeTags);
    _bindings.free_audio_tags(nativeTags);
    return tags;
  }

  bool setAudioTags(String filename, Tags tags) {
    Pointer<taglib.Tags> nativeTags = tags.toNativeTags();
    int rc = _bindings.set_audio_tags(
        filename.toNativeUtf8().cast<Char>(), nativeTags);
    calloc.free(nativeTags);
    return rc != 0;
  }

  Future<Uint8List?> getArtworkBytes(String filename) async {
    final SendPort helperIsolateSendPort = await _helperIsolateSendPort;
    final int requestId = _nextRequestId++;
    final _ArtworkRequest request = _ArtworkRequest(requestId, filename);
    final Completer<taglib.Artwork> completer = Completer<taglib.Artwork>();
    _artworkRequests[requestId] = completer;
    helperIsolateSendPort.send(request);
    return completer.future.then((nativeArtwork) {
      if (nativeArtwork.buffer == nullptr) {
        return null;
      } else {
        Uint8List nativeBytes =
            nativeArtwork.buffer.cast<Uint8>().asTypedList(nativeArtwork.size);
        Uint8List bytes = Uint8List.fromList(nativeBytes);
        _bindings.free_artwork(nativeArtwork);
        return bytes;
      }
    });
  }

  bool setArtwork(String filename, Uint8List bytes) {
    // 1st convert buffer
    final Pointer<Uint8> pointer = malloc.allocate(bytes.length);
    pointer.asTypedList(bytes.length).setRange(0, bytes.length, bytes);

    final Pointer<taglib.Artwork> artwork = calloc<taglib.Artwork>();
    artwork.ref.buffer = pointer as Pointer<UnsignedChar>;
    artwork.ref.size = bytes.lengthInBytes;
    int rc =
        _bindings.set_artwork(filename.toNativeUtf8().cast<Char>(), artwork);
    malloc.free(pointer);
    calloc.free(artwork);
    return rc != 0;
  }

  Future<String> getLyrics(String filename) async {
    final SendPort helperIsolateSendPort = await _helperIsolateSendPort;
    final int requestId = _nextRequestId++;
    final _LyricsRequest request = _LyricsRequest(requestId, filename);
    final Completer<taglib.Lyrics> completer = Completer<taglib.Lyrics>();
    _lyricsRequests[requestId] = completer;
    helperIsolateSendPort.send(request);
    return completer.future.then((nativeLyrics) {
      return fromNativeString(nativeLyrics.lyrics);
    });
  }

  bool setLyrics(String filename, String lyrics) {
    final Pointer<taglib.Lyrics> nativeLyrics = calloc<taglib.Lyrics>();
    nativeLyrics.ref.lyrics = lyrics.toNativeUtf8().cast<Char>();
    int rc = _bindings.set_lyrics(
        filename.toNativeUtf8().cast<Char>(), nativeLyrics);
    calloc.free(nativeLyrics);
    return rc != 0;
  }
}

class _ArtworkRequest {
  final int id;
  final String filename;
  const _ArtworkRequest(this.id, this.filename);
}

class _ArtworkResponse {
  final int id;
  final taglib.Artwork artwork;
  _ArtworkResponse(this.id, this.artwork);
}

class _LyricsRequest {
  final int id;
  final String filename;
  const _LyricsRequest(this.id, this.filename);
}

class _LyricsResponse {
  final int id;
  final taglib.Lyrics lyrics;
  _LyricsResponse(this.id, this.lyrics);
}

/// Counter to identify [_SumRequest]s and [_SumResponse]s.
int _nextRequestId = 0;

/// Mapping from request `id`s to the completers corresponding to the correct future of the pending request.
final Map<int, Completer<taglib.Artwork>> _artworkRequests =
    <int, Completer<taglib.Artwork>>{};
final Map<int, Completer<taglib.Lyrics>> _lyricsRequests =
    <int, Completer<taglib.Lyrics>>{};

/// The SendPort belonging to the helper isolate.
Future<SendPort> _helperIsolateSendPort = () async {
  // The helper isolate is going to send us back a SendPort, which we want to
  // wait for.
  final Completer<SendPort> completer = Completer<SendPort>();

  // Receive port on the main isolate to receive messages from the helper.
  // We receive two types of messages:
  // 1. A port to send messages on.
  // 2. Responses to requests we sent.
  final ReceivePort receivePort = ReceivePort()
    ..listen((dynamic data) {
      if (data is SendPort) {
        // The helper isolate sent us the port on which we can sent it requests.
        completer.complete(data);
        return;
      }
      if (data is _ArtworkResponse) {
        // The helper isolate sent us a response to a request we sent.
        final Completer<taglib.Artwork> completer = _artworkRequests[data.id]!;
        _artworkRequests.remove(data.id);
        completer.complete(data.artwork);
        return;
      }
      if (data is _LyricsResponse) {
        // The helper isolate sent us a response to a request we sent.
        final Completer<taglib.Lyrics> completer = _lyricsRequests[data.id]!;
        _artworkRequests.remove(data.id);
        completer.complete(data.lyrics);
        return;
      }
      throw UnsupportedError('Unsupported message type: ${data.runtimeType}');
    });

  // Start the helper isolate.
  await Isolate.spawn((SendPort sendPort) async {
    final ReceivePort helperReceivePort = ReceivePort()
      ..listen((dynamic data) {
        // On the helper isolate listen to requests and respond to them.
        if (data is _ArtworkRequest) {
          final taglib.Artwork result =
              _bindings.get_artwork(data.filename.toNativeUtf8().cast<Char>());
          final _ArtworkResponse response = _ArtworkResponse(data.id, result);
          sendPort.send(response);
          return;
        }
        if (data is _LyricsRequest) {
          final taglib.Lyrics result =
              _bindings.get_lyrics(data.filename.toNativeUtf8().cast<Char>());
          final _LyricsResponse response = _LyricsResponse(data.id, result);
          sendPort.send(response);
          return;
        }
        throw UnsupportedError('Unsupported message type: ${data.runtimeType}');
      });

    // Send the the port to the main isolate on which we can receive requests.
    sendPort.send(helperReceivePort.sendPort);
  }, receivePort.sendPort);

  // Wait until the helper isolate has sent us back the SendPort on which we
  // can start sending requests.
  return completer.future;
}();
