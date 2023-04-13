import 'dart:io';
import 'dart:typed_data';

import 'package:test/test.dart';
import 'package:taglib_ffi/src/main.dart';

import 'utils.dart';

void testUpdateArtwork(String filename, String artworkSource) async {
  // copy file
  String tempCopy = copyFile(filename);

  // load artwork
  TagLib tagLib = TagLib(path: '../src', test: true);
  Uint8List? referenceBytes = await tagLib.getArtworkBytes(artworkSource);

  // update
  expect(tagLib.setArtwork(tempCopy, referenceBytes!), true);

  // check
  Uint8List? updatedBytes = await tagLib.getArtworkBytes(tempCopy);
  expect(updatedBytes, referenceBytes);

  // done
  File(tempCopy).deleteSync();
}

void testReadArtwork() {
  test('mp3', () async {
    TagLib tagLib = TagLib(path: '../src', test: true);
    Uint8List? artwork = await tagLib.getArtworkBytes('../data/sample.mp3');
    expect(artwork, isNotNull);
    expect(artwork!.length, 867);
    expect(artwork.buffer.asUint8List(0, 4), [0x89, 0x50, 0x4e, 0x47]);
  });

  test('flac', () async {
    TagLib tagLib = TagLib(path: '../src', test: true);
    Uint8List? artwork = await tagLib.getArtworkBytes('../data/sample.flac');
    expect(artwork, isNotNull);
    expect(artwork!.length, 741);
    expect(artwork.buffer.asUint8List(0, 4), [0x89, 0x50, 0x4e, 0x47]);
  });

  test('m4a', () async {
    TagLib tagLib = TagLib(path: '../src', test: true);
    Uint8List? artwork = await tagLib.getArtworkBytes('../data/sample.m4a');
    expect(artwork, isNotNull);
    expect(artwork!.length, 820);
    expect(artwork.buffer.asUint8List(0, 4), [0x89, 0x50, 0x4e, 0x47]);
  });
}

void testWriteArtwork() {
  test('mp3', () {
    testUpdateArtwork('../data/sample.mp3', '../data/sample.flac');
  });

  test('flac', () {
    testUpdateArtwork('../data/sample.flac', '../data/sample.m4a');
  });

  test('m4a', () {
    testUpdateArtwork('../data/sample.m4a', '../data/sample.mp3');
  });
}
