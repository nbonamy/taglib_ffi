// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:ffi';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
import '../taglib_ffi_bindings_generated.dart' as taglib;
import '../src/model.dart';
import '../src/utils.dart';

class TagLib {
  late taglib.TaglibFfiBindings _bindings;
  TagLib({String path = '.', bool test = false}) {
    _bindings = taglib.TaglibFfiBindings(loadLibrary(path, test));
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

  Uint8List? getArtworkBytes(String filename) {
    taglib.Artwork nativeArtwork =
        _bindings.get_artwork(filename.toNativeUtf8().cast<Char>());
    if (nativeArtwork.buffer == nullptr) {
      return null;
    } else {
      Uint8List nativeBytes =
          nativeArtwork.buffer.cast<Uint8>().asTypedList(nativeArtwork.size);
      Uint8List bytes = Uint8List.fromList(nativeBytes);
      _bindings.free_artwork(nativeArtwork);
      return bytes;
    }
  }
}
