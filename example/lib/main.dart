import 'dart:typed_data';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:taglib_ffi/taglib_ffi.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final TagLib _tagLib = TagLib();
  String? _filename;
  Uint8List? _artwork;
  Tags? _tags;
  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('TagLib FFI'),
        ),
        body: SingleChildScrollView(
          child: Container(
            width: double.infinity,
            padding: const EdgeInsets.all(10),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.center,
              children: [
                ElevatedButton(
                    child: const Text('Select a file...'),
                    onPressed: () async {
                      FilePickerResult? result =
                          await FilePicker.platform.pickFiles();
                      if (result != null) {
                        setState(() {
                          _filename = result.files.single.path;
                          _tags = _tagLib.getAudioTags(_filename!);
                          _artwork = _tagLib.getArtworkBytes(_filename!);
                        });
                      }
                    }),
                if (_filename != null) ...[
                  const SizedBox(height: 32),
                  Text(
                    _filename!,
                    style: const TextStyle(fontWeight: FontWeight.bold),
                  ),
                  const SizedBox(height: 16),
                  if (_artwork != null) ...[
                    Image.memory(_artwork!),
                    const SizedBox(height: 16),
                  ],
                  if (_tags != null) ...[
                    if (!_tags!.valid)
                      const Text('No tags')
                    else ...[
                      Text(_tags!.title),
                      Text(_tags!.album),
                      Text(_tags!.artist),
                      Text(_tags!.performer),
                      Text(_tags!.genre),
                      Text(_tags!.comments),
                      Text(_tags!.year.toString()),
                      Text(_tags!.compilation.toString()),
                      Text(_tags!.volumeIndex.toString()),
                      Text(_tags!.trackIndex.toString()),
                      Text(_tags!.duration.toString()),
                      Text(_tags!.numChannels.toString()),
                      Text(_tags!.sampleRate.toString()),
                      Text(_tags!.bitsPerSample.toString()),
                      Text(_tags!.bitrate.toString()),
                    ],
                  ],
                ],
              ],
            ),
          ),
        ),
      ),
    );
  }
}
