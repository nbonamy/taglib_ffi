# taglib_ffi

A dart wrapper of TagLib.

**Only tested on MacOS for now!**

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

To read tags:

```dart
TagLib tagLib = TagLib();
Tags tags = tagLib.getAudioTags(filename);
if (tags.valid) {
  ...
}
```

To display artwork:

```dart
TagLib tagLib = TagLib();
Uint8List? artwork = tagLib.getArtworkBytes(filename);
-> Image.memory(_artwork!)
```

## Tests

```shell
cd src
cmake .
make
cd ../test
dart main.dart
```
