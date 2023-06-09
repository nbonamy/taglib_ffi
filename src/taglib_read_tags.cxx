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

using namespace TagLib;

char* copy_taglib_str(const String &str) {
  size_t length = strlen(str.to8Bit(true).c_str());
  char* buffer = (char*) malloc(length+1);
  memcpy(buffer, str.to8Bit(true).c_str(), length);
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

String get_tag_value(ID3v2::Tag *tag, const char *tag_name) {
  if (tag == NULL) return "";
  ID3v2::FrameList l = tag->frameList(tag_name);
  if (!l.isEmpty()) {
    String value = l.front()->toString();
    return value;
  }
  return "";
}

int int_from_pair(String value, int index) {
  StringList ints = value.split("/");
  if (ints.size() > index) return ints[index].toInt();
  else return 0;
}

void fetch_audio_properties(AudioProperties *properties, struct Tags &tags) {

  // check
  if (properties == NULL) {
    //std::cout << "no audio properties found\n";
    return;
  }
  
  // basic
  tags.duration = properties->lengthInSeconds();
  tags.num_channels = properties->channels();
  tags.bitrate = properties->bitrate();
  tags.sample_rate = properties->sampleRate();

  // flac
  FLAC::Properties* flacProperties = dynamic_cast<FLAC::Properties*>(properties);
  if (flacProperties != NULL) {
    tags.bits_per_sample = flacProperties->bitsPerSample();
  }

  // mp4
  MP4::Properties* mp4Properties = dynamic_cast<MP4::Properties*>(properties);
  if (mp4Properties != NULL) {
    tags.bits_per_sample = mp4Properties->bitsPerSample();
  }

}

void fetch_basic_info(Tag *libtags, struct Tags &tags) {
  tags.title = copy_taglib_str(libtags->title());
  tags.artist = copy_taglib_str(libtags->artist());
  tags.album = copy_taglib_str(libtags->album());
  tags.performer = copy_taglib_str(libtags->artist());
  tags.genre = copy_taglib_str(libtags->genre());
  tags.comment = copy_taglib_str(libtags->comment());
  tags.year = libtags->year();
  tags.track_index = libtags->track();
}

String get_xiph_comment(Ogg::XiphComment *xiphComments, const char *key) {
  const Ogg::FieldListMap &fieldListMap = xiphComments->fieldListMap();
  Ogg::FieldListMap::ConstIterator it = fieldListMap.find(key);
  if (it != fieldListMap.end()) {
    StringList stringList = it->second;
    return stringList.toString();
  } else {
    return "";
  }
}

void get_audio_info_from_tags(ID3v2::Tag *id3v2tag, Ogg::XiphComment *xiphComments, struct Tags &tags) {

  if (id3v2tag && id3v2tag->isEmpty() == false) {

    // log
    //std::cout << "parsing id3v2 tags\n";

    // get info
    // artist = TPE2 = Album Artist in iTunes
    // performer = TPE1 = Artist in iTunes
    tags.title = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_TITLE));
    tags.album = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_ALBUM));
    tags.artist = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_BAND));
    tags.performer = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_LEADARTIST));
    tags.composer = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_COMPOSER));
    tags.genre = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_CONTENTTYPE));
    tags.copyright = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_COPYRIGHT));
    tags.comment = copy_taglib_str(get_tag_value(id3v2tag, ID3TID_COMMENT));
    tags.year = get_tag_value(id3v2tag, ID3TID_RECORDINGTIME).toInt();
    tags.volume_index = int_from_pair(get_tag_value(id3v2tag, ID3TID_PARTINSET), 0);
    tags.volume_count = int_from_pair(get_tag_value(id3v2tag, ID3TID_PARTINSET), 1);
    tags.track_index = int_from_pair(get_tag_value(id3v2tag, ID3TID_TRACKNUM), 0);
    tags.track_count = int_from_pair(get_tag_value(id3v2tag, ID3TID_TRACKNUM), 1);

    // misc stuff
    if (tags.year == 0) {
      tags.year = get_tag_value(id3v2tag, ID3TID_RECORDINGDATES).toInt();
    }

    // compilation
    tags.compilation = get_tag_value(id3v2tag, "TCMP").toInt() == 1;
    if (tags.compilation == false) {
      tags.compilation = get_tag_value(id3v2tag, "TCP").toInt() == 1;
    }

    // done
    tags.valid = true;
 
  } else if (xiphComments && xiphComments->isEmpty() == false) {

    // log
    //std::cout << "parsing vorbis comments\n";

    // basic
    fetch_basic_info(xiphComments, tags);

    // additional info
    tags.artist = copy_taglib_str(get_xiph_comment(xiphComments, "ALBUMARTIST"));
    tags.performer = copy_taglib_str(get_xiph_comment(xiphComments, "ARTIST"));
    tags.composer = copy_taglib_str(get_xiph_comment(xiphComments, "COMPOSER"));
    tags.copyright = copy_taglib_str(get_xiph_comment(xiphComments, "COPYRIGHT"));
    tags.comment = copy_taglib_str(get_xiph_comment(xiphComments, "COMMENT"));
    tags.compilation = get_xiph_comment(xiphComments, "COMPILATION").toInt() == 1;
    tags.volume_index = get_xiph_comment(xiphComments, "DISCNUMBER").toInt();
    tags.volume_count = get_xiph_comment(xiphComments, "DISCTOTAL").toInt();
    tags.track_count = get_xiph_comment(xiphComments, "TRACKTOTAL").toInt();

    // done
    tags.valid = true;

  } else {

    //std::cout << "no id3v2 tags or vorbis comments found\n";

  }

}

void get_audio_tags_mpeg(const char *filename, struct Tags &tags) {

  // log
  //std::cout << "opening mpeg file\n" ;

  // open the file
  MPEG::File f(filename);
  if (f.isValid() == false) {
    //std::cout << "unable to open file\n";
    return;
  }

  // get id3v2
  ID3v2::Tag* id3v2tag = f.ID3v2Tag();

  // do it
  fetch_audio_properties(f.audioProperties(), tags);
  get_audio_info_from_tags(id3v2tag, NULL, tags);

}

void get_audio_tags_flac(const char *filename, struct Tags &tags) {

  // log
  //std::cout << "opening flac file\n" ;

  // open the file
  FLAC::File f(filename);
  if (f.isValid() == false) {
    //std::cout << "unable to open file\n";
    return;
  }

  // get tags
  ID3v2::Tag* id3v2tag = f.ID3v2Tag();
  Ogg::XiphComment* xiphComments = f.xiphComment();

  // do it
  fetch_audio_properties(f.audioProperties(), tags);
  get_audio_info_from_tags(id3v2tag, xiphComments, tags);

}

String get_mp4_tag_string(MP4::Tag *mp4tag, const char *key) {
  if (mp4tag->contains(key)) {
    MP4::Item item = mp4tag->item(key);
    return item.toStringList().toString();
  } else {
    return "";
  }
}

int get_mp4_tag_int(MP4::Tag *mp4tag, const char *key, int index = 0) {
  if (mp4tag->contains(key)) {
    MP4::Item item = mp4tag->item(key);
    MP4::Item::IntPair pair = item.toIntPair();
    if (index == 1) return pair.second;
    else return pair.first;
  } else {
    return 0;
  }
}

void get_audio_tags_mp4(const char *filename, struct Tags &tags) {

  // log
  //std::cout << "opening mp4 file\n" ;

  // open file
  MP4::File f(filename);
  if (f.isValid() == false) {
    //std::cout << "unable to open file\n";
    return;
  }

  // tags
  MP4::Tag *mp4tag = f.tag();  
  if (mp4tag == NULL || mp4tag->isEmpty()) {
    //std::cout << "no mp4 tags found\n";
    return;
  }

  // log
  //std::cout << "parsing mp4 tags\n";

  // basic stuff
  fetch_audio_properties(f.audioProperties(), tags);
  fetch_basic_info(mp4tag, tags);

  // additional info
  tags.artist = copy_taglib_str(get_mp4_tag_string(mp4tag, "aART"));
  tags.performer = copy_taglib_str(get_mp4_tag_string(mp4tag, "\251ART"));
  tags.composer = copy_taglib_str(get_mp4_tag_string(mp4tag, "\251wrt"));
  tags.copyright = copy_taglib_str(get_mp4_tag_string(mp4tag, "cprt"));
  tags.comment = copy_taglib_str(get_mp4_tag_string(mp4tag, "\251cmt"));
  tags.compilation = get_mp4_tag_int(mp4tag, "cpil") == 1;
  tags.volume_index = get_mp4_tag_int(mp4tag, "disk", 0);
  tags.volume_count = get_mp4_tag_int(mp4tag, "disk", 1);
  tags.track_count = get_mp4_tag_int(mp4tag, "trkn", 1);

  // done
  tags.valid = true;
}

void get_audio_tags_default(const char *filename, struct Tags &tags) {
  
  // log
  //std::cout << "opening generic file\n" ;

  // open file
  File *f = FileRef::create(filename);
  if (f == NULL || f->isValid() == false) {
    //std::cout << "unable to open file\n";
    return;
  }

  // tags
  Tag *tag = f->tag();
  if (tag == NULL) {
    //std::cout << "no tags found\n";
    return;
  }

  // log
  //std::cout << "parsing generic tags\n";

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
    get_audio_tags_mpeg(filename, tags);
  } else if (ext == ".flac") {
    get_audio_tags_flac(filename, tags);
  } else if (ext == ".m4a") {
    get_audio_tags_mp4(filename, tags);
  }

  // default
  if (tags.valid == false) {
    get_audio_tags_default(filename, tags);
  }

  // fix some stuff
  if (tags.valid) {

    // artist/performer
    if (tags.artist == NULL || strlen(tags.artist) == 0) {
      tags.artist = copy_string(tags.performer);
    }
    if (tags.performer == NULL || strlen(tags.performer) == 0) {
      tags.performer = copy_string(tags.artist);
    }

    // year
    if (tags.year > 0 && tags.year < 100) {
      tags.year = 2000 + tags.year;
    }

    // genre
    std::string genre_str_id = tags.genre;
    if (genre_str_id.empty() == false) {
      // remove parenthesis
      genre_str_id.erase(0, genre_str_id.find_first_not_of(")"));
      genre_str_id.erase(genre_str_id.find_last_not_of("(") + 1);
      bool is_only_digits = std::all_of(genre_str_id.begin(), genre_str_id.end(), ::isdigit);
      if (is_only_digits) {
        uint genre_id = atoi(genre_str_id.c_str());
        String taglib_genre = ID3v1::genre(genre_id);
        if (taglib_genre.isEmpty() == false) {
          SAFE_FREE(tags.genre);
          tags.genre = copy_taglib_str(taglib_genre);
        }
      }
    }
  }

  // log
  if (!tags.valid) {
    //std::cout << "no audio tags found\n";
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
