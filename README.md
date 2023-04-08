# taglib_ffi

A dart wrapper of TagLib.

**Only tested on MacOS for now!**

## Dependencies

### MacOS

```
brew install cmake
brew install taglib
```

## Usage

Add as a `pubspec.yaml` dependency.

To read tags:

```
TagLib tagLib = TagLib();
Tags tags = tagLib.getAudioTags(filename);
if (tags.valid) {
  ...
}
```

## Tests

```
cd src
cmake .
make
cd ../test
dart main.dart
```

