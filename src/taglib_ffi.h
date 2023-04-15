#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#if _WIN32
#define FFI_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FFI_PLUGIN_EXPORT
#endif

#ifndef __cplusplus
#define bool int
#endif

#define SAFE_FREE(x) if (x != NULL) free(x)

#ifdef __cplusplus
extern "C" {
#endif

  struct Tags {
    bool valid;
    char* title;
    char* album;
    char* artist;
    char* performer;
    char* composer;
    char* genre;
    char* copyright;
    char* comment;
    unsigned int year;
    bool compilation;
    int volume_index;
    int volume_count;
    int track_index;
    int track_count;
    int duration;
    int num_channels;
    int sample_rate;
    int bits_per_sample;
    int bitrate;
  };

  struct Lyrics {
    char* lyrics;
  };

  struct Artwork {
    unsigned char* buffer;
    size_t size;
  };

  FFI_PLUGIN_EXPORT struct Tags get_audio_tags(const char*);
  FFI_PLUGIN_EXPORT int set_audio_tags(const char*, struct Tags*);
  FFI_PLUGIN_EXPORT void free_audio_tags(struct Tags);

  FFI_PLUGIN_EXPORT struct Lyrics get_lyrics(const char*);
  FFI_PLUGIN_EXPORT int set_lyrics(const char*, struct Lyrics*);
  FFI_PLUGIN_EXPORT void free_lyrics(struct Lyrics);

  FFI_PLUGIN_EXPORT struct Artwork get_artwork(const char*);
  FFI_PLUGIN_EXPORT int set_artwork(const char*, struct Artwork*);
  FFI_PLUGIN_EXPORT void free_artwork(struct Artwork);

#ifdef __cplusplus
}
#endif
