// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'package:test/test.dart';
import 'package:taglib_ffi/taglib_ffi.dart' as lib;

void main() {
  test('mp3', () {
    lib.TagLib tagLib = lib.TagLib(path: '../src', test: true);
    lib.Tags tags = tagLib.getAudioTags('../data/sample.mp3');
    expect(tags.valid, true);
    expect(tags.title, 'Title');
    expect(tags.album, 'Album');
    expect(tags.artist, 'Artist');
    expect(tags.performer, 'Performer');
    expect(tags.genre, 'Acid');
    expect(tags.year, 1900);
    expect(tags.compilation, true);
    expect(tags.volumeIndex, 1);
    expect(tags.trackIndex, 2);
    expect(tags.duration, 5);
    expect(tags.numChannels, 2);
    expect(tags.bitsPerSample, 0);
    expect(tags.sampleRate, 44100);
    expect(tags.bitrate, 32);
  });

  test('flac', () {
    lib.TagLib tagLib = lib.TagLib(path: '../src', test: true);
    lib.Tags tags = tagLib.getAudioTags('../data/sample.flac');
    expect(tags.valid, true);
    expect(tags.title, 'Title');
    expect(tags.album, 'Album');
    expect(tags.artist, 'Artist');
    expect(tags.performer, 'Performer');
    expect(tags.genre, 'Acid');
    expect(tags.year, 1900);
    expect(tags.compilation, true);
    expect(tags.volumeIndex, 1);
    expect(tags.trackIndex, 2);
    expect(tags.duration, 5);
    expect(tags.numChannels, 2);
    expect(tags.bitsPerSample, 24);
    expect(tags.sampleRate, 48000);
    //expect(tags.bitrate, 1);
  });

  test('m4a', () {
    lib.TagLib tagLib = lib.TagLib(path: '../src', test: true);
    lib.Tags tags = tagLib.getAudioTags('../data/sample.m4a');
    expect(tags.valid, true);
    expect(tags.title, 'Title');
    expect(tags.album, 'Album');
    expect(tags.artist, 'Artist');
    expect(tags.performer, 'Performer');
    expect(tags.genre, 'Acid');
    expect(tags.year, 1900);
    expect(tags.compilation, true);
    expect(tags.volumeIndex, 1);
    expect(tags.trackIndex, 2);
    expect(tags.duration, 5);
    expect(tags.numChannels, 2);
    expect(tags.bitsPerSample, 16);
    expect(tags.sampleRate, 44100);
    //expect(tags.bitrate, 3);
  });
}
