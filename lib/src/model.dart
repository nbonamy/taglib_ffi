// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:ffi';
import 'package:ffi/ffi.dart';
import '../taglib_ffi_bindings_generated.dart' as taglib;

class Tags {
  bool valid;
  String title;
  String album;
  String artist;
  String performer;
  String composer;
  String genre;
  String copyright;
  String comment;
  int year;
  bool compilation;
  int volumeIndex;
  int volumeCount;
  int trackIndex;
  int trackCount;
  int duration;
  int numChannels;
  int sampleRate;
  int bitsPerSample;
  int bitrate;

  Tags({
    this.valid = false,
    this.title = '',
    this.album = '',
    this.artist = '',
    this.performer = '',
    this.composer = '',
    this.genre = '',
    this.copyright = '',
    this.comment = '',
    this.year = 0,
    this.compilation = false,
    this.volumeIndex = 0,
    this.volumeCount = 0,
    this.trackIndex = 0,
    this.trackCount = 0,
    this.duration = 0,
    this.numChannels = 0,
    this.sampleRate = 0,
    this.bitsPerSample = 0,
    this.bitrate = 0,
  });

  factory Tags.copy(Tags other) {
    return Tags(
      valid: other.valid,
      title: other.title,
      album: other.album,
      artist: other.artist,
      performer: other.performer,
      composer: other.composer,
      genre: other.genre,
      copyright: other.copyright,
      comment: other.comment,
      year: other.year,
      compilation: other.compilation,
      volumeIndex: other.volumeIndex,
      volumeCount: other.volumeCount,
      trackIndex: other.trackIndex,
      trackCount: other.trackCount,
      duration: other.duration,
      numChannels: other.numChannels,
      sampleRate: other.sampleRate,
      bitsPerSample: other.bitsPerSample,
      bitrate: other.bitrate,
    );
  }

  factory Tags.fromNativeTags(taglib.Tags tags) {
    if (tags.valid == 0) {
      return Tags();
    }

    return Tags(
      valid: true,
      title: _fromNativeString(tags.title),
      album: _fromNativeString(tags.album),
      artist: _fromNativeString(tags.artist),
      performer: _fromNativeString(tags.performer),
      composer: _fromNativeString(tags.composer),
      genre: _fromNativeString(tags.genre),
      copyright: _fromNativeString(tags.copyright),
      comment: _fromNativeString(tags.comment),
      year: tags.year,
      compilation: tags.compilation == 1,
      volumeIndex: tags.volume_index,
      volumeCount: tags.volume_count,
      trackIndex: tags.track_index,
      trackCount: tags.track_count,
      duration: tags.duration,
      numChannels: tags.num_channels,
      sampleRate: tags.sample_rate,
      bitsPerSample: tags.bits_per_sample,
      bitrate: tags.bitrate,
    );
  }

  Pointer<taglib.Tags> toNativeTags() {
    final Pointer<taglib.Tags> tags = calloc<taglib.Tags>();
    tags.ref.valid = valid ? 1 : 0;
    tags.ref.title = title.toNativeUtf8().cast<Char>();
    tags.ref.album = album.toNativeUtf8().cast<Char>();
    tags.ref.artist = artist.toNativeUtf8().cast<Char>();
    tags.ref.performer = performer.toNativeUtf8().cast<Char>();
    tags.ref.composer = composer.toNativeUtf8().cast<Char>();
    tags.ref.genre = genre.toNativeUtf8().cast<Char>();
    tags.ref.copyright = copyright.toNativeUtf8().cast<Char>();
    tags.ref.comment = comment.toNativeUtf8().cast<Char>();
    tags.ref.year = year;
    tags.ref.compilation = compilation ? 1 : 0;
    tags.ref.volume_index = volumeIndex;
    tags.ref.volume_count = volumeCount;
    tags.ref.track_index = trackIndex;
    tags.ref.track_count = trackCount;
    return tags;
  }

  bool equals(Tags? other) {
    if (other == null) return false;
    return valid == other.valid &&
        title == other.title &&
        album == other.album &&
        artist == other.artist &&
        performer == other.performer &&
        composer == other.composer &&
        genre == other.genre &&
        copyright == other.copyright &&
        comment == other.comment &&
        year == other.year &&
        compilation == other.compilation &&
        volumeIndex == other.volumeIndex &&
        volumeCount == other.volumeCount &&
        trackIndex == other.trackIndex &&
        trackCount == other.trackCount &&
        duration == other.duration &&
        numChannels == other.numChannels &&
        sampleRate == other.sampleRate &&
        bitsPerSample == other.bitsPerSample &&
        bitrate == other.bitrate;
  }

  static String _fromNativeString(Pointer<Char> ptr) {
    return ptr == nullptr ? '' : ptr.cast<Utf8>().toDartString();
  }
}
