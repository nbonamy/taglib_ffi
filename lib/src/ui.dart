import 'dart:typed_data';
import 'dart:ui' as ui;
import 'main.dart';

extension TagLibUtils on TagLib {
  Future<ui.Image?> getArtwork(String filename) async {
    Uint8List? imgBytes = getArtworkBytes(filename);
    if (imgBytes == null) return null;
    ui.Codec codec = await ui.instantiateImageCodec(imgBytes);
    ui.FrameInfo frame = await codec.getNextFrame();
    return frame.image;
  }
}
