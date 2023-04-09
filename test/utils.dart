import 'dart:io';
import 'package:path/path.dart' as p;

String copyFile(String filename) {
  Directory tempDir = Directory.systemTemp;
  String tempFilename = p.join(tempDir.path, 'test${p.extension(filename)}');
  File(filename).copySync(tempFilename);
  return tempFilename;
}
