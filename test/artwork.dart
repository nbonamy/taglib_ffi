import 'dart:typed_data';

import 'package:test/test.dart';
import 'package:taglib_ffi/src/main.dart';

void testArtwork() {
  test('mp3', () {
    TagLib tagLib = TagLib(path: '../src', test: true);
    Uint8List? artwork = tagLib.getArtworkBytes('../data/sample.mp3');
    expect(artwork, isNotNull);
    expect(artwork!.length, 867);
    expect(artwork.buffer.asUint8List(0, 4), [0x89, 0x50, 0x4e, 0x47]);
  });

  test('flac', () {
    TagLib tagLib = TagLib(path: '../src', test: true);
    Uint8List? artwork = tagLib.getArtworkBytes('../data/sample.flac');
    expect(artwork, isNotNull);
    expect(artwork!.length, 741);
    expect(artwork.buffer.asUint8List(0, 4), [0x89, 0x50, 0x4e, 0x47]);
  });

  test('m4a', () {
    TagLib tagLib = TagLib(path: '../src', test: true);
    Uint8List? artwork = tagLib.getArtworkBytes('../data/sample.m4a');
    expect(artwork, isNotNull);
    expect(artwork!.length, 820);
    expect(artwork.buffer.asUint8List(0, 4), [0x89, 0x50, 0x4e, 0x47]);
  });
}
