import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

const String _libName = 'taglib_ffi';
DynamicLibrary loadLibrary(String path, bool test) {
  String fullpath = '';

  if (Platform.isMacOS || Platform.isIOS) {
    if (test) {
      fullpath = '$path/lib$_libName.dylib';
    } else {
      fullpath = '$_libName.framework/$_libName';
    }
  } else if (Platform.isAndroid || Platform.isLinux) {
    fullpath = '$path/lib$_libName.so';
  } else if (Platform.isWindows) {
    fullpath = '$path/$_libName.dll';
  } else {
    throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
  }

  if (File(fullpath).existsSync()) {
    return DynamicLibrary.open(fullpath);
  } else {
    return DynamicLibrary.executable();
  }
}

String fromNativeString(Pointer<Char> ptr) {
  return ptr == nullptr ? '' : ptr.cast<Utf8>().toDartString();
}
