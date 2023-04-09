import 'package:test/test.dart';
import 'package:taglib_ffi/src/model.dart' as lib;
import 'package:taglib_ffi/src/main.dart';

void checkMetadata(lib.Tags tags) {
  expect(tags.valid, true);
  expect(tags.title, 'Title');
  expect(tags.album, 'Album');
  expect(tags.artist, 'Artist');
  expect(tags.performer, 'Performer');
  expect(tags.composer, 'Composer');
  expect(tags.genre, 'Acid');
  expect(tags.copyright, '2023 Copyright');
  expect(tags.comment,
      'Dès Noël où un zéphyr haï me vêt de glaçons würmiens je dîne d’exquis rôtis de bœuf au kir à l’aÿ d’âge mûr & cætera !');
  expect(tags.year, 1900);
  expect(tags.compilation, true);
  expect(tags.volumeIndex, 1);
  expect(tags.trackIndex, 2);
}

void testReadTags() {
  test('mp3', () {
    TagLib tagLib = TagLib(path: '../src', test: true);
    lib.Tags tags = tagLib.getAudioTags('../data/sample.mp3');
    checkMetadata(tags);
    expect(tags.duration, 5);
    expect(tags.numChannels, 2);
    expect(tags.bitsPerSample, 0);
    expect(tags.sampleRate, 44100);
    expect(tags.bitrate, 32);
  });

  test('flac', () {
    TagLib tagLib = TagLib(path: '../src', test: true);
    lib.Tags tags = tagLib.getAudioTags('../data/sample.flac');
    checkMetadata(tags);
    expect(tags.duration, 5);
    expect(tags.numChannels, 2);
    expect(tags.bitsPerSample, 24);
    expect(tags.sampleRate, 48000);
    //expect(tags.bitrate, 1);
  });

  test('m4a', () {
    TagLib tagLib = TagLib(path: '../src', test: true);
    lib.Tags tags = tagLib.getAudioTags('../data/sample.m4a');
    checkMetadata(tags);
    expect(tags.duration, 5);
    expect(tags.numChannels, 2);
    expect(tags.bitsPerSample, 16);
    expect(tags.sampleRate, 44100);
    //expect(tags.bitrate, 3);
  });
}
