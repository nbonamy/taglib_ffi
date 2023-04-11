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
Uint8List? artwork = tagLib.getArtworkBytes(filename);
-> Image.memory(_artwork!)
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
