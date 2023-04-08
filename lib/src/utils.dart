import 'dart:ffi';
import 'dart:io';

const String _libName = 'taglib_ffi';
DynamicLibrary loadLibrary(String path, bool test) {
  if (Platform.isMacOS || Platform.isIOS) {
    if (test) {
      return DynamicLibrary.open('$path/lib$_libName.dylib');
    } else {
      return DynamicLibrary.open('$_libName.framework/$_libName');
    }
  }
  if (Platform.isAndroid || Platform.isLinux) {
    return DynamicLibrary.open('$path/lib$_libName.so');
  }
  if (Platform.isWindows) {
    return DynamicLibrary.open('$path/$_libName.dll');
  }
  throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
}
