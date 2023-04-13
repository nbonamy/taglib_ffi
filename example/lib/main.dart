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
                  FutureBuilder(
                      future: _tagLib.getArtworkBytes(_filename!),
                      builder: (context, snapshot) {
                        if (snapshot.hasData && snapshot.data != null) {
                          return Container(
                            margin: const EdgeInsets.only(bottom: 16),
                            child: Image.memory(
                              snapshot.data!,
                              height: 64,
                            ),
                          );
                        } else {
                          return Container();
                        }
                      }),
                  if (_tags != null) ...[
                    if (!_tags!.valid)
                      const Text('No tags')
                    else ...[
                      Text('Title: ${_tags!.title}'),
                      Text('Album: ${_tags!.album}'),
                      Text('Artist: ${_tags!.artist}'),
                      Text('Performer: ${_tags!.performer}'),
                      Text('Composer: ${_tags!.composer}'),
                      Text('Genre: ${_tags!.genre}'),
                      Text('Copyright: ${_tags!.copyright}'),
                      Text('Comment: ${_tags!.comment}'),
                      Text('Year: ${_tags!.year}'),
                      Text('Compilation: ${_tags!.compilation}'),
                      Text(
                          'Volume: ${_tags!.volumeIndex} / ${_tags!.volumeCount}'),
                      Text(
                          'Track: ${_tags!.trackIndex} / ${_tags!.trackCount}'),
                      Text('Duration: ${_tags!.duration}'),
                      Text('Chanels: ${_tags!.numChannels}'),
                      Text('Sample Rate: ${_tags!.sampleRate}'),
                      Text('Bits per Sample: ${_tags!.bitsPerSample}'),
                      Text('Bitrate: ${_tags!.bitrate}'),
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
