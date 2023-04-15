import 'dart:io';

import 'package:test/test.dart';
import 'package:taglib_ffi/src/main.dart';

import 'utils.dart';

void testUpdateLyrics(String filename, String lyrics) {
  // copy file
  String tempCopy = copyFile(filename);

  // update
  TagLib tagLib = TagLib(path: '../src', test: true);
  expect(tagLib.setLyrics(tempCopy, lyrics), true);

  // check
  String updatedLyrics = tagLib.getLyrics(tempCopy);
  expect(lyrics, updatedLyrics);

  // done
  File(tempCopy).deleteSync();
}

void testReadLyrics() {}

void testWriteLyrics() {
  test('mp3', () {
    testUpdateLyrics('../data/sample.mp3',
        'Dès Noël où un zéphyr haï me vêt de glaçons würmiens je dîne d’exquis rôtis de bœuf au kir à l’aÿ d’âge mûr & cætera !');
  });

  test('flac', () {
    testUpdateLyrics('../data/sample.flac',
        'Dès Noël où un zéphyr haï me vêt de glaçons würmiens je dîne d’exquis rôtis de bœuf au kir à l’aÿ d’âge mûr & cætera !');
  });

  test('m4a', () {
    testUpdateLyrics('../data/sample.m4a',
        'Dès Noël où un zéphyr haï me vêt de glaçons würmiens je dîne d’exquis rôtis de bœuf au kir à l’aÿ d’âge mûr & cætera !');
  });
}
