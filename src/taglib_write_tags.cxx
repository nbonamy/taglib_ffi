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
#include "taglib/commentsframe.h"
#include "taglib/textidentificationframe.h"
#include "taglib/tpropertymap.h"
#include "taglib/xiphcomment.h"

#include "id3_tags.h"

using namespace TagLib;

void delete_id3v2_frame(ID3v2::Tag* id3v2tag, const char *frame_name) {
  ID3v2::FrameList frames = id3v2tag->frameList(frame_name);
  for (ID3v2::FrameList::Iterator it = frames.begin(); it != frames.end(); ++it) {
    id3v2tag->removeFrame(*it);
  }
}

void update_id3v2_frame_str(ID3v2::Tag* id3v2tag, const char *frame_name, const char *value) {

  // first delete
  delete_id3v2_frame(id3v2tag, frame_name);
  if (value == NULL || strlen(value) == 0) {
    return;
  }

  // now create new frame
  ID3v2::TextIdentificationFrame* frame = new ID3v2::TextIdentificationFrame(ByteVector(frame_name), String::UTF8);
  frame->setText(String(value, String::UTF8));
  id3v2tag->addFrame(frame);

}

void update_id3v2_frame_int(ID3v2::Tag* id3v2tag, const char *frame_name, int value) {

  // build string value
  const size_t k_max_size = 256;
  char str_value[k_max_size];
  memset(str_value, 0, k_max_size);
  if (value > 0) {
    snprintf(str_value, k_max_size, "%d", value);
  }

  // now do it
  update_id3v2_frame_str(id3v2tag, frame_name, str_value);

}

void update_id3v2_tags(ID3v2::Tag* id3v2tag, struct Tags *tags) {

  // update simple frames
  update_id3v2_frame_str(id3v2tag, ID3TID_TITLE, tags->title);
  update_id3v2_frame_str(id3v2tag, ID3TID_ALBUM, tags->album);
  update_id3v2_frame_str(id3v2tag, ID3TID_BAND, tags->artist);
  update_id3v2_frame_str(id3v2tag, ID3TID_LEADARTIST, tags->performer);
  update_id3v2_frame_str(id3v2tag, ID3TID_COMPOSER, tags->composer);
  update_id3v2_frame_str(id3v2tag, ID3TID_CONTENTTYPE, tags->genre);
  update_id3v2_frame_str(id3v2tag, ID3TID_COPYRIGHT, tags->copyright);
  update_id3v2_frame_int(id3v2tag, ID3TID_RECORDINGTIME, tags->year);
  update_id3v2_frame_int(id3v2tag, "TCMP", tags->compilation);
  update_id3v2_frame_int(id3v2tag, ID3TID_PARTINSET, tags->volume_index);
  update_id3v2_frame_int(id3v2tag, ID3TID_TRACKNUM, tags->track_index);

  // comment is different
  delete_id3v2_frame(id3v2tag, ID3TID_COMMENT);
  if (tags->comment != NULL && strlen(tags->comment) != 0) {
    ID3v2::CommentsFrame* frame = new ID3v2::CommentsFrame(String::UTF8);
    frame->setText(String(tags->comment, String::UTF8));
    id3v2tag->addFrame(frame);
  }

}

void update_vorbis_field_str(Ogg::XiphComment *xiphComments, const char *field_name, const char *value) {

  // delete
  xiphComments->removeFields(field_name);
  if (value == NULL || strlen(value) == 0) {
    return;
  }

  // add
  xiphComments->addField(field_name, String(value, String::UTF8));

}

void update_vorbis_field_int(Ogg::XiphComment *xiphComments, const char *field_name, int value) {

  // build string value
  const size_t k_max_size = 256;
  char str_value[k_max_size];
  memset(str_value, 0, k_max_size);
  if (value > 0) {
    snprintf(str_value, k_max_size, "%d", value);
  }

  // now do it
  update_vorbis_field_str(xiphComments, field_name, str_value);

}

void update_vorbis_comments(Ogg::XiphComment *xiphComments, struct Tags *tags) {

  update_vorbis_field_str(xiphComments, "TITLE", tags->title);
  update_vorbis_field_str(xiphComments, "ALBUM", tags->album);
  update_vorbis_field_str(xiphComments, "ALBUMARTIST", tags->artist);
  update_vorbis_field_str(xiphComments, "ARTIST", tags->performer);
  update_vorbis_field_str(xiphComments, "COMPOSER", tags->composer);
  update_vorbis_field_str(xiphComments, "GENRE", tags->genre);
  update_vorbis_field_str(xiphComments, "COPYRIGHT", tags->copyright);
  update_vorbis_field_str(xiphComments, "COMMENT", tags->comment);
  update_vorbis_field_int(xiphComments, "DATE", tags->year);
  update_vorbis_field_int(xiphComments, "COMPILATION", tags->compilation);
  update_vorbis_field_int(xiphComments, "DISCNUMBER", tags->volume_index);
  update_vorbis_field_int(xiphComments, "TRACKNUMBER", tags->track_index);

}

void update_mp4_item_str(MP4::Tag *mp4tag, const char *item_name, const char *value) {

  // delete
  mp4tag->removeItem(item_name);
  if (value == NULL || strlen(value) == 0) {
    return;
  }

  // add
  mp4tag->setItem(item_name, MP4::Item(String(value, String::UTF8)));

}

void update_mp4_item_str(MP4::Tag *mp4tag, const char *item_name, int value) {

  // build string value
  const size_t k_max_size = 256;
  char str_value[k_max_size];
  memset(str_value, 0, k_max_size);
  if (value > 0) {
    snprintf(str_value, k_max_size, "%d", value);
  }

  // now do it
  update_mp4_item_str(mp4tag, item_name, str_value);

}

void update_mp4_item_int(MP4::Tag *mp4tag, const char *item_name, int value) {

  // delete
  mp4tag->removeItem(item_name);
  if (value <= 0) {
    return;
  }

  // now do it
  mp4tag->setItem(item_name, MP4::Item(value));

}

int set_audio_tags_mpeg(const char *filename, struct Tags *tags) {

  // log
  std::cout << "opening mpeg file\n" ;

  // open the file
  MPEG::File f(filename);
  if (f.isValid() == false) {
    std::cout << "unable to open file\n";
    return 0;
  }

  // get/create id3v2 and update it
  ID3v2::Tag* id3v2tag = f.ID3v2Tag(true);
  update_id3v2_tags(id3v2tag, tags);

  // log
  std::cout << "saving mpeg file\n" ;

  // done
  return f.save();

}

int set_audio_tags_flac(const char *filename, struct Tags *tags) {

  // log
  std::cout << "opening flac file\n" ;

  // open the file
  FLAC::File f(filename);
  if (f.isValid() == false) {
    std::cout << "unable to open file\n";
    return 0;
  }

  // get tags
  ID3v2::Tag* id3v2tag = f.ID3v2Tag();
  Ogg::XiphComment* xiphComments = f.xiphComment();

  // if id3v2 exists update it
  if (id3v2tag != NULL) {
    update_id3v2_tags(id3v2tag, tags);
  }

  // re-get to ensure they are created
  xiphComments = f.xiphComment(true);
  update_vorbis_comments(xiphComments, tags);

  // log
  std::cout << "saving flac file\n" ;

  // done
  return f.save();
}

int set_audio_tags_mp4(const char *filename, struct Tags *tags) {

  // log
  std::cout << "opening mp4 file\n" ;

  // open file
  MP4::File f(filename);
  if (f.isValid() == false) {
    std::cout << "unable to open file\n";
    return 0;
  }

  // tags
  MP4::Tag *mp4tag = f.tag();  
  if (mp4tag == NULL) {
    std::cout << "no mp4 tags found\n";
    return 0;
  }

  // update
  update_mp4_item_str(mp4tag, "\251nam", tags->title);
  update_mp4_item_str(mp4tag, "\251alb", tags->album);
  update_mp4_item_str(mp4tag, "aART", tags->artist);
  update_mp4_item_str(mp4tag, "\251ART", tags->performer);
  update_mp4_item_str(mp4tag, "\251wrt", tags->composer);
  update_mp4_item_str(mp4tag, "\251gen", tags->genre);
  update_mp4_item_str(mp4tag, "cprt", tags->copyright);
  update_mp4_item_str(mp4tag, "\251cmt", tags->comment);
  update_mp4_item_str(mp4tag, "\251day", tags->year);
  update_mp4_item_int(mp4tag, "cpil", tags->compilation);
  update_mp4_item_int(mp4tag, "disk", tags->volume_index);
  update_mp4_item_int(mp4tag, "trkn", tags->track_index);

  // log
  std::cout << "saving mp4 file\n" ;

  // done
  return f.save();
}

int set_audio_tags_default(const char *filename, struct Tags *tags) {
  return 0;
}

FFI_PLUGIN_EXPORT int set_audio_tags(const char *filename, struct Tags *tags) {

  // based on extension
  std::string ext = strrchr(filename, '.');
  if (ext == ".mp3") {
    return set_audio_tags_mpeg(filename, tags);
  } else if (ext == ".flac") {
    return set_audio_tags_flac(filename, tags);
  } else if (ext == ".m4a") {
    return set_audio_tags_mp4(filename, tags);
  } else {
    return set_audio_tags_default(filename, tags);
  }

}
