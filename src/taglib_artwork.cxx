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
#include "taglib/attachedpictureframe.h"

#include "id3_tags.h"

char* copy_buffer(const char *src, size_t length) {
  if (src == NULL) return NULL;
  char* dest = (char*) malloc(length);
  memcpy((char*) dest, (char*) src, length);
  return dest;
}

FFI_PLUGIN_EXPORT struct Artwork get_artwork(const char* filename) {

  // init
  struct Artwork artwork;
  memset(&artwork, 0, sizeof(artwork));
  
  // open a file stream so we can manage r/w access
  TagLib::FileStream fileStream(filename, true);
  if (fileStream.isOpen() == false) {
    std::cout << "unable to open file\n";
  }

  // mpeg?
  {
    TagLib::MPEG::File mpegfile(&fileStream, TagLib::ID3v2::FrameFactory::instance());
    if (mpegfile.ID3v2Tag()) {

      // log
      std::cout << "parsing id3v2 tags\n";

      // get picture
      TagLib::ID3v2::FrameList fl = mpegfile.ID3v2Tag()->frameListMap()[ID3TID_PICTURE];
      if (!fl.isEmpty()) {
        TagLib::ID3v2::AttachedPictureFrame* p = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(fl.front());
        if (p != NULL) {
          artwork.buffer = (unsigned char*) copy_buffer(p->picture().data(), p->picture().size());
          artwork.size = p->picture().size();
          return artwork;
        }
      }
    }
  }

  // mp4
  {
    TagLib::MP4::File mp4file(&fileStream);
    if (mp4file.isValid()) {
      TagLib::MP4::Tag* mp4tag = mp4file.tag();
      if (mp4tag != NULL) {

        // log
        std::cout << "parsing mp4 tags\n";

        // do it
        if (mp4tag->contains("covr")) {
          TagLib::MP4::Item item = mp4tag->item("covr");
          if (item.isValid()) {
            TagLib::MP4::CoverArtList artlist = item.toCoverArtList();
            for (TagLib::MP4::CoverArtList::ConstIterator it = artlist.begin(); it != artlist.end(); ++it) {
              const TagLib::MP4::CoverArt& coverart = *it;
              artwork.buffer = (unsigned char*) copy_buffer(coverart.data().data(), coverart.data().size());
              artwork.size = coverart.data().size();
              return artwork;
            }
          }
        }
      }
    }
  }

  // flac
  {
    TagLib::FLAC::File flacfile(&fileStream, TagLib::ID3v2::FrameFactory::instance());

    // start with FLAC picture list then Vorbis picture list
    TagLib::List<TagLib::FLAC::Picture*> pictureList = flacfile.pictureList();
    if (pictureList.isEmpty() && flacfile.xiphComment()) {
      TagLib::List<TagLib::FLAC::Picture*> pictureList = flacfile.xiphComment()->pictureList();
    }
    TagLib::FLAC::Picture* picture = pictureList.front();
    for (TagLib::List<TagLib::FLAC::Picture*>::Iterator it = pictureList.begin(); it != pictureList.end(); ++it) {
      if ((*it)->type() == TagLib::FLAC::Picture::FrontCover) {
        picture = *it;
      }
    }
    if (picture != NULL) {
      artwork.buffer = (unsigned char*) copy_buffer(picture->data().data(), picture->data().size());
      artwork.size = picture->data().size();
      return artwork;
    }

    // then id3v2
    if (flacfile.ID3v2Tag()) {
      TagLib::ID3v2::FrameList fl = flacfile.ID3v2Tag()->frameListMap()["APIC"];
      if (!fl.isEmpty()) {
        TagLib::ID3v2::AttachedPictureFrame* p = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(fl.front());
        if (p != NULL) {
          artwork.buffer = (unsigned char*) copy_buffer(p->picture().data(), p->picture().size());
          artwork.size = p->picture().size();
          return artwork;
        }
      }
    }
  }

  // done
  return artwork;
}

FFI_PLUGIN_EXPORT void free_artwork(struct Artwork artwork) {
  SAFE_FREE(artwork.buffer);
}