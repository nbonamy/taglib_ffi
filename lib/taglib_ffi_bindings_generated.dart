// ignore_for_file: always_specify_types
// ignore_for_file: camel_case_types
// ignore_for_file: non_constant_identifier_names

// AUTO GENERATED FILE, DO NOT EDIT.
//
// Generated by `package:ffigen`.
import 'dart:ffi' as ffi;

/// Bindings for `src/taglib_ffi.h`.
/// Regenerate bindings with `flutter pub run ffigen --config ffigen.yaml`.
///
class TaglibFfiBindings {
  /// Holds the symbol lookup function.
  final ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
      _lookup;

  /// The symbols are looked up in [dynamicLibrary].
  TaglibFfiBindings(ffi.DynamicLibrary dynamicLibrary)
      : _lookup = dynamicLibrary.lookup;

  /// The symbols are looked up with [lookup].
  TaglibFfiBindings.fromLookup(
      ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
          lookup)
      : _lookup = lookup;

  Tags get_audio_tags(
    ffi.Pointer<ffi.Char> arg0,
  ) {
    return _get_audio_tags(
      arg0,
    );
  }

  late final _get_audio_tagsPtr =
      _lookup<ffi.NativeFunction<Tags Function(ffi.Pointer<ffi.Char>)>>(
          'get_audio_tags');
  late final _get_audio_tags =
      _get_audio_tagsPtr.asFunction<Tags Function(ffi.Pointer<ffi.Char>)>();

  void free_audio_tags(
    Tags arg0,
  ) {
    return _free_audio_tags(
      arg0,
    );
  }

  late final _free_audio_tagsPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(Tags)>>('free_audio_tags');
  late final _free_audio_tags =
      _free_audio_tagsPtr.asFunction<void Function(Tags)>();

  Artwork get_artwork(
    ffi.Pointer<ffi.Char> arg0,
  ) {
    return _get_artwork(
      arg0,
    );
  }

  late final _get_artworkPtr =
      _lookup<ffi.NativeFunction<Artwork Function(ffi.Pointer<ffi.Char>)>>(
          'get_artwork');
  late final _get_artwork =
      _get_artworkPtr.asFunction<Artwork Function(ffi.Pointer<ffi.Char>)>();

  void free_artwork(
    Artwork arg0,
  ) {
    return _free_artwork(
      arg0,
    );
  }

  late final _free_artworkPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(Artwork)>>('free_artwork');
  late final _free_artwork =
      _free_artworkPtr.asFunction<void Function(Artwork)>();
}

class Tags extends ffi.Struct {
  @ffi.Int()
  external int valid;

  external ffi.Pointer<ffi.Char> title;

  external ffi.Pointer<ffi.Char> album;

  external ffi.Pointer<ffi.Char> artist;

  external ffi.Pointer<ffi.Char> performer;

  external ffi.Pointer<ffi.Char> composer;

  external ffi.Pointer<ffi.Char> genre;

  external ffi.Pointer<ffi.Char> copyright;

  external ffi.Pointer<ffi.Char> comment;

  @ffi.UnsignedInt()
  external int year;

  @ffi.Int()
  external int compilation;

  @ffi.Int()
  external int volume_index;

  @ffi.Int()
  external int track_index;

  @ffi.Int()
  external int duration;

  @ffi.Int()
  external int num_channels;

  @ffi.Int()
  external int sample_rate;

  @ffi.Int()
  external int bits_per_sample;

  @ffi.Int()
  external int bitrate;
}

class Artwork extends ffi.Struct {
  external ffi.Pointer<ffi.UnsignedChar> buffer;

  @ffi.Size()
  external int size;
}
