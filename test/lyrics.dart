import 'dart:io';

import 'package:test/test.dart';
import 'package:taglib_ffi/src/main.dart';

import 'utils.dart';

void testUpdateLyrics(String filename, String lyrics) async {
  // copy file
  String tempCopy = copyFile(filename);

  // update
  TagLib tagLib = TagLib(path: '../src', test: true);
  expect(tagLib.setLyrics(tempCopy, lyrics), true);

  // check
  String updatedLyrics = await tagLib.getLyrics(tempCopy);
  expect(lyrics, updatedLyrics);

  // done
  File(tempCopy).deleteSync();
}

void testReadLyrics() {
  test('mp3', () async {
    TagLib tagLib = TagLib(path: '../src', test: true);
    String lyrics = await tagLib.getLyrics('../data/sample.mp3');
    expect(lyrics,
        'All I ever needed\nIs here in my arms\nWords are very unnecessary\nThey can only do harm');
  });

  test('flac', () async {
    TagLib tagLib = TagLib(path: '../src', test: true);
    String lyrics = await tagLib.getLyrics('../data/sample.flac');
    expect(lyrics,
        'All I ever needed\nIs here in my arms\nWords are very unnecessary\nThey can only do harm');
  });

  test('m4a', () async {
    TagLib tagLib = TagLib(path: '../src', test: true);
    String lyrics = await tagLib.getLyrics('../data/sample.m4a');
    expect(lyrics,
        'All I ever needed\nIs here in my arms\nWords are very unnecessary\nThey can only do harm');
  });
}

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
