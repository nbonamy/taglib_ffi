import 'dart:io';

import 'package:test/test.dart';
import 'package:taglib_ffi/src/model.dart' as lib;
import 'package:taglib_ffi/src/main.dart';

import 'utils.dart';

void testUpdateTags(String filename) {
  // copy file
  String tempCopy = copyFile(filename);

  // load current tags
  TagLib tagLib = TagLib(path: '../src', test: true);
  lib.Tags tags = tagLib.getAudioTags(tempCopy);

  // update values
  tags.title = 'Titre';
  tags.album = 'Disque';
  tags.artist = 'Artiste';
  tags.performer = 'Interprète';
  tags.composer = 'Compositeur';
  tags.genre = 'Genre';
  tags.copyright = '2023 Droits restreints';
  tags.comment = 'Commentaire';
  tags.year = 2023;
  tags.compilation = false;
  tags.volumeIndex = 2;
  tags.trackIndex = 3;

  // now run it
  expect(tagLib.setAudioTags(tempCopy, tags), true);

  // re-read and compare
  lib.Tags tags2 = tagLib.getAudioTags(tempCopy);
  expect(tags2.title, tags.title);
  expect(tags2.album, tags.album);
  expect(tags2.artist, tags.artist);
  expect(tags2.performer, tags.performer);
  expect(tags2.composer, tags.composer);
  expect(tags2.performer, tags.performer);
  expect(tags2.genre, tags.genre);
  expect(tags2.copyright, tags.copyright);
  expect(tags2.comment, tags.comment);
  expect(tags2.year, tags.year);
  expect(tags2.compilation, tags.compilation);
  expect(tags2.volumeIndex, tags.volumeIndex);
  expect(tags2.trackIndex, tags.trackIndex);

  // done
  File(tempCopy).deleteSync();
}

void testDeleteTags(String filename) {
  // copy file
  String tempCopy = copyFile(filename);

  // load current tags
  TagLib tagLib = TagLib(path: '../src', test: true);
  lib.Tags tags = tagLib.getAudioTags(tempCopy);

  // update values
  tags.copyright = '';
  tags.comment = '';
  tags.volumeIndex = 0;

  // now run it
  expect(tagLib.setAudioTags(tempCopy, tags), true);

  // re-read and compare
  lib.Tags tags2 = tagLib.getAudioTags(tempCopy);
  expect(tags2.title, tags.title);
  expect(tags2.album, tags.album);
  expect(tags2.artist, tags.artist);
  expect(tags2.performer, tags.performer);
  expect(tags2.composer, tags.composer);
  expect(tags2.performer, tags.performer);
  expect(tags2.genre, tags.genre);
  expect(tags2.copyright, tags.copyright);
  expect(tags2.comment, tags.comment);
  expect(tags2.year, tags.year);
  expect(tags2.compilation, tags.compilation);
  expect(tags2.volumeIndex, tags.volumeIndex);
  expect(tags2.trackIndex, tags.trackIndex);

  // done
  File(tempCopy).deleteSync();
}

void testWriteTags() {
  test('mp3', () {
    testUpdateTags('../data/sample.mp3');
    testDeleteTags('../data/sample.mp3');
  });

  test('flac', () {
    testUpdateTags('../data/sample.flac');
    testDeleteTags('../data/sample.flac');
  });

  test('m4a', () {
    testUpdateTags('../data/sample.m4a');
    testDeleteTags('../data/sample.m4a');
  });
}
