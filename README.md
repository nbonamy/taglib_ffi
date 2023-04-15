# taglib_ffi

<hr/>

### **Only built/tested on MacOS for now!**
<hr/>

A dart wrapper of TagLib.

Supported formats:
- MP3
- FLAC
- MP4 (aka. M4A, aka. ALAC)

Supported fiunctionalities:
- Read/Update/Delete tags (see below)
- Read/Update/Delete artwork
- Read/Update/Delete lyrics

## Dependencies

### MacOS

```shell
brew install cmake
brew install taglib
```

## Setup

### MacOS

`HOMEBREW_PREFIX` variable needs to be defined:

```shell
export HOMEBREW_PREFIX="$(brew --prefix)" 
```

## Usage

Add as a `pubspec.yaml` dependency.

To read/write tags:

```dart
TagLib tagLib = TagLib();
Tags tags = tagLib.getAudioTags(filename);
if (tags.valid) {
  ...
}
// update tags
tagLib.setAudioTag(filename, tags);
```

To display artwork:

```dart
TagLib tagLib = TagLib();
FutureBuilder(
  future: tagLib.getArtworkBytes(filename),
  builder: (_, snapshot) => Image.memory(snapshot.data)
)
```

To read/write lyrics:
```dart
TagLib tagLib = TagLib();
String lyrics = tagLib.getLyrics(filename);
// update lyrics
tagLib.setLyrics(filename, lyrics);
```

## Tests

```shell
make tests
```

## Supported tags

- Title
- Album
- Artist (Album Artist)
- Performer
- Composer
- Genre
- Copyright
- Comment
- Year
- Compilation
- Volume/Album Index
- Volume/Album Count
- Track Index
- Track Count
