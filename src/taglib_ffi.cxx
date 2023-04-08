#include "taglib_ffi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "taglib/fileref.h"
#include "taglib/mpegfile.h"
#include "taglib/mp4file.h"
#include "taglib/flacfile.h"
#include "taglib/tfilestream.h"
#include "taglib/id3v2tag.h"
#include "taglib/id3v2framefactory.h"
#include "taglib/id3v1genres.h"
#include "taglib/mp4tag.h"
#include "taglib/tpropertymap.h"
#include "taglib/xiphcomment.h"

#include "id3_tags.h"

#define SAFE_FREE(x) if (x != NULL) free(x)

char* copy_taglib_str(const TagLib::String &str) {
  size_t length = str.length();
  char* buffer = (char*) malloc(length+1);
  strncpy(buffer, str.to8Bit(true).c_str(), length);
  buffer[length] = '\0';
  return buffer;
}

char* copy_string(const char *src) {
  if (src == NULL) return copy_string("");
  size_t length = strlen(src);
  char* dest = (char*) malloc(length+1);
  strncpy(dest, src, length);
  dest[length] = '\0';
  return dest;
}

TagLib::String get_tag_value(TagLib::ID3v2::Tag *tag, const char *tag_name) {
  if (tag == NULL) return "";
  TagLib::ID3v2::FrameList l = tag->frameList(tag_name);
  if (!l.isEmpty()) {
    TagLib::String value = l.front()->toString();
    return value;
  }
  return "";
}

void fetch_audio_properties(TagLib::AudioProperties *properties, struct Tags &tags) {

  // check
  if (properties == NULL) {
    std::cout << "no audio properties found\n";
    return;
  }
  
  // basic
  tags.duration = properties->lengthInSeconds();
  tags.num_channels = properties->channels();
  tags.bitrate = properties->bitrate();
  tags.sample_rate = properties->sampleRate();

  // flac
  TagLib::FLAC::Properties* flacProperties = dynamic_cast<TagLib::FLAC::Properties*>(properties);
  if (flacProperties != NULL) {
    tags.bits_per_sample = flacProperties->bitsPerSample();
  }

  // mp4
  TagLib::MP4::Properties* mp4Properties = dynamic_cast<TagLib::MP4::Properties*>(properties);
  if (mp4Properties != NULL) {
    tags.bits_per_sample = mp4Properties->bitsPerSample();
  }

}

void fetch_basic_info(TagLib::Tag *libtags, struct Tags &tags) {
  tags.title = copy_taglib_str(libtags->title());
  tags.artist = copy_taglib_str(libtags->artist());
  tags.album = copy_taglib_str(libtags->album());
  tags.performer = copy_taglib_str(libtags->artist());
  tags.genre = copy_taglib_str(libtags->genre());
  tags.year = libtags->year();
  tags.track_index = libtags->track();
}

TagLib::String get_xiph_comment(TagLib::Ogg::XiphComment *xiphComments, const char *key) {
  const TagLib::Ogg::FieldListMap &fieldListMap = xiphComments->fieldListMap();
  TagLib::Ogg::FieldListMap::ConstIterator it = fieldListMap.find(key);
  if (it != fieldListMap.end()) {
    TagLib::StringList stringList = it->second;
    return stringList.toString();
  } else {
    return "";
  }
}

void get_audio_info_from_tags(TagLib::ID3v2::Tag *id3v2tag, TagLib::Ogg::XiphComment *xiphComments, struct Tags &tags) {

  if (id3v2tag && id3v2tag->isEmpty() == false) {

    // log
    std::cout << "parsing id3v2 tags\n";

    // get info
    // artist = TPE2 = Album Artist in iTunes
    // performer = TPE1 = Artist in iTunes
    tags.title = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_TITLE));
    tags.artist = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_BAND));
    tags.performer = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_LEADARTIST));
    tags.album = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_ALBUM));
    tags.genre = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_CONTENTTYPE));
    tags.year = get_tag_value(id3v2tag, ID3TID_RECORDINGTIME).toInt();
    tags.volume_index = get_tag_value(id3v2tag, ID3TID_PARTINSET).toInt();
    tags.track_index = get_tag_value(id3v2tag, ID3TID_TRACKNUM).toInt();

    // misc stuff
    if (tags.year == 0) {
      tags.year = get_tag_value(id3v2tag, ID3TID_RECORDINGDATES).toInt();
    }

    // compilation
    std::string tcmp = get_tag_value(id3v2tag, "TCMP").toCString();
    tags.compilation = (tcmp.empty() == false && tcmp != "0");
    if (tags.compilation == false) {
      std::string tcp = get_tag_value(id3v2tag, "TCP").toCString();
      tags.compilation = (tcp.empty() == false && tcmp != "0");
    }

    // done
    tags.valid = true;
 
  } else if (xiphComments && xiphComments->isEmpty() == false) {

    // log
    std::cout << "parsing vorbis comments\n";

    // basic
    fetch_basic_info(xiphComments, tags);

    // additional info
    tags.artist = copy_taglib_str(get_xiph_comment(xiphComments, "ALBUMARTIST"));
    tags.performer = copy_taglib_str(get_xiph_comment(xiphComments, "ARTIST"));
    tags.compilation = get_xiph_comment(xiphComments, "COMPILATION").toInt() == 1;
    tags.volume_index = get_xiph_comment(xiphComments, "DISCNUMBER").toInt();

    // done
    tags.valid = true;

  } else {

    std::cout << "no id3v2 tags or vorbis comments found\n";

  }

}

void get_audio_tags_mp3(const char *filename, struct Tags &tags) {

  // log
  std::cout << "reading mp3 tags\n" ;

  // open the file
  TagLib::FileStream fileStream(filename, true);
  TagLib::MPEG::File f(&fileStream, TagLib::ID3v2::FrameFactory::instance());
  if (f.isValid() == false) {
    std::cout << "unable to open file\n";
    return;
  }

  // get id3v2
  TagLib::ID3v2::Tag* id3v2tag = f.ID3v2Tag();

  // do it
  fetch_audio_properties(f.audioProperties(), tags);
  get_audio_info_from_tags(id3v2tag, NULL, tags);

}

void get_audio_tags_flac(const char *filename, struct Tags &tags) {

  // log
  std::cout << "reading flac tags\n" ;

  // open the file
  TagLib::FileStream fileStream(filename, true);
  TagLib::FLAC::File f(&fileStream, TagLib::ID3v2::FrameFactory::instance());
  if (f.isValid() == false) {
    std::cout << "unable to open file\n";
    return;
  }

  // get tags
  TagLib::ID3v2::Tag* id3v2tag = f.ID3v2Tag();
  TagLib::Ogg::XiphComment* xiphComments = f.xiphComment();

  // do it
  fetch_audio_properties(f.audioProperties(), tags);
  get_audio_info_from_tags(id3v2tag, xiphComments, tags);

}

TagLib::String get_mp4_tag_string(TagLib::MP4::Tag *mp4tag, const char *key) {
  if (mp4tag->contains(key)) {
    TagLib::MP4::Item item = mp4tag->item(key);
    return item.toStringList().toString();
  } else {
    return "";
  }
}

int get_mp4_tag_int(TagLib::MP4::Tag *mp4tag, const char *key) {
  if (mp4tag->contains(key)) {
    TagLib::MP4::Item item = mp4tag->item(key);
    return item.toInt();
  } else {
    return 0;
  }
}

void get_audio_tags_mp4(const char *filename, struct Tags &tags) {

  // log
  std::cout << "reading mp4 tags\n" ;

  // open file
  TagLib::FileStream fileStream(filename, true);
  TagLib::MP4::File f(&fileStream);
  if (f.isValid() == false) {
    std::cout << "unable to open file\n";
    return;
  }

  // tags
  TagLib::MP4::Tag *mp4tag = f.tag();  
  if (mp4tag == NULL || mp4tag->isEmpty()) {
    std::cout << "no mp4 tags found\n";
    return;
  }

  // basic stuff
  fetch_audio_properties(f.audioProperties(), tags);
  fetch_basic_info(mp4tag, tags);

  // additional info
  tags.artist = copy_taglib_str(get_mp4_tag_string(mp4tag, "aART"));
  tags.performer = copy_taglib_str(get_mp4_tag_string(mp4tag, "\251ART"));
  tags.compilation = get_mp4_tag_int(mp4tag, "cpil") == 1;
  tags.volume_index = get_mp4_tag_int(mp4tag, "disk");

  // done
  tags.valid = true;
}

void get_audio_tags_default(const char *filename, struct Tags &tags) {
  
  // log
  std::cout << "reading default tags\n" ;

  // open file
  TagLib::File *f = TagLib::FileRef::create(filename);
  if (f == NULL || f->isValid() == false) {
    std::cout << "unable to open file\n";
    return;
  }

  // tags
  TagLib::Tag *tag = f->tag();
  if (tag == NULL) {
    std::cout << "no tags found\n";
    return;
  }

  // basic stuff
  fetch_audio_properties(f->audioProperties(), tags);
  fetch_basic_info(tag, tags);

  // done
  tags.valid = true;

}

FFI_PLUGIN_EXPORT struct Tags get_audio_tags(const char *filename) {

  // init
  struct Tags tags;
  memset(&tags, 0, sizeof(tags));
    
  // based on extension
  std::string ext = strrchr(filename, '.');
  if (ext == ".mp3") {
    get_audio_tags_mp3(filename, tags);
  } else if (ext == ".flac") {
    get_audio_tags_flac(filename, tags);
  } else if (ext == ".m4a") {
    get_audio_tags_mp4(filename, tags);
  }

  // default
  if (tags.valid == false) {
    get_audio_tags_default(filename, tags);
  }

  // artist/performer
  if (tags.valid) {
    if (tags.artist == NULL || strlen(tags.artist) == 0) {
      tags.artist = copy_string(tags.performer);
    }
    if (tags.performer == NULL || strlen(tags.performer) == 0) {
      tags.performer = copy_string(tags.artist);
    }
  }

  // log
  if (!tags.valid) {
    std::cout << "no audio tags found\n";
  }
  
  // done
  return tags;

}

FFI_PLUGIN_EXPORT void free_audio_tags(struct Tags tags) {
  SAFE_FREE(tags.title);
  SAFE_FREE(tags.artist);
  SAFE_FREE(tags.album);
  SAFE_FREE(tags.performer);
  SAFE_FREE(tags.genre);
}
