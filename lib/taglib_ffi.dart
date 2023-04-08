// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:ffi';
import 'dart:io' show Platform;
import 'package:ffi/ffi.dart';
import 'taglib_ffi_bindings_generated.dart' as taglib;

class Tags {
  bool valid;
  String title;
  String artist;
  String album;
  String performer;
  String genre;
  int year;
  bool compilation;
  int volumeIndex;
  int trackIndex;
  int duration;
  int numChannels;
  int sampleRate;
  int bitsPerSample;
  int bitrate;

  Tags({
    this.valid = false,
    this.title = '',
    this.artist = '',
    this.album = '',
    this.performer = '',
    this.genre = '',
    this.year = 0,
    this.compilation = false,
    this.volumeIndex = 0,
    this.trackIndex = 0,
    this.duration = 0,
    this.numChannels = 0,
    this.sampleRate = 0,
    this.bitsPerSample = 0,
    this.bitrate = 0,
  });

  factory Tags.fromNativeTags(taglib.Tags tags) {
    if (tags.valid == 0) {
      return Tags();
    }

    return Tags(
      valid: true,
      title: _fromNativeString(tags.title),
      artist: _fromNativeString(tags.artist),
      album: _fromNativeString(tags.album),
      performer: _fromNativeString(tags.performer),
      genre: _fromNativeString(tags.genre),
      year: tags.year,
      compilation: tags.compilation == 1,
      volumeIndex: tags.volume_index,
      trackIndex: tags.track_index,
      duration: tags.duration,
      numChannels: tags.num_channels,
      sampleRate: tags.sample_rate,
      bitsPerSample: tags.bits_per_sample,
      bitrate: tags.bitrate,
    );
  }

  static String _fromNativeString(Pointer<Char> ptr) {
    return ptr == nullptr ? '' : ptr.cast<Utf8>().toDartString();
  }
}

class TagLib {
  final String _libName = 'taglib_ffi';
  late taglib.TaglibFfiBindings _bindings;
  TagLib({
    String path = '.',
    bool test = false,
  }) {
    _bindings = taglib.TaglibFfiBindings(_dylib(
      path,
      test: test,
    ));
  }

  Tags getAudioTags(String filename) {
    taglib.Tags nativeTags =
        _bindings.get_audio_tags(filename.toNativeUtf8().cast<Char>());
    Tags tags = Tags.fromNativeTags(nativeTags);
    _bindings.free_audio_tags(nativeTags);
    return tags;
  }

  DynamicLibrary _dylib(String path, {bool test = false}) {
    if (Platform.isMacOS || Platform.isIOS) {
      if (test) {
        return DynamicLibrary.open('$path/lib$_libName.dylib');
      } else {
        return DynamicLibrary.open('$_libName.framework/$_libName');
      }
    }
    if (Platform.isAndroid || Platform.isLinux) {
      return DynamicLibrary.open('$path/lib$_libName.so');
    }
    if (Platform.isWindows) {
      return DynamicLibrary.open('$path/$_libName.dll');
    }
    throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
  }
}
