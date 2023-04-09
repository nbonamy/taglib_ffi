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

#include "taglib_write_tags.h"

using namespace TagLib;

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
  FileStream fileStream(filename, true);
  if (fileStream.isOpen() == false) {
    std::cout << "unable to open file\n";
  }

  // mp4
  {
    MP4::File mp4file(&fileStream);
    if (mp4file.isValid() && mp4file.tag()) {

      // log
      std::cout << "trying mp4 cover\n";

      // do it
      if (mp4file.tag()->contains("covr")) {
        MP4::Item item = mp4file.tag()->item("covr");
        if (item.isValid()) {
          MP4::CoverArtList artlist = item.toCoverArtList();
          for (MP4::CoverArtList::ConstIterator it = artlist.begin(); it != artlist.end(); ++it) {
            std::cout << "got mp4 cover\n";
            const MP4::CoverArt& coverart = *it;
            artwork.buffer = (unsigned char*) copy_buffer(coverart.data().data(), coverart.data().size());
            artwork.size = coverart.data().size();
            return artwork;
          }
        }
      }
    }
  }

  // flac
  {
    FLAC::File flacfile(&fileStream, ID3v2::FrameFactory::instance());
    if (flacfile.isValid()) {

      // log
      std::cout << "trying flac picture\n";

      // start with FLAC picture list then Vorbis picture list
      List<FLAC::Picture*> pictureList = flacfile.pictureList();
      if (pictureList.isEmpty() && flacfile.xiphComment()) {
        List<FLAC::Picture*> pictureList = flacfile.xiphComment()->pictureList();
      }
      FLAC::Picture* picture = pictureList.front();
      for (List<FLAC::Picture*>::Iterator it = pictureList.begin(); it != pictureList.end(); ++it) {
        if ((*it)->type() == FLAC::Picture::FrontCover) {
          picture = *it;
        }
      }
      if (picture != NULL) {
        std::cout << "got ogg@flac picture\n";
        artwork.buffer = (unsigned char*) copy_buffer(picture->data().data(), picture->data().size());
        artwork.size = picture->data().size();
        return artwork;
      }

      // then id3v2
      if (flacfile.ID3v2Tag()) {
        ID3v2::FrameList fl = flacfile.ID3v2Tag()->frameListMap()["APIC"];
        if (!fl.isEmpty()) {
          ID3v2::AttachedPictureFrame* p = static_cast<ID3v2::AttachedPictureFrame*>(fl.front());
          if (p != NULL) {
            std::cout << "got id3v2@flac picture\n";
            artwork.buffer = (unsigned char*) copy_buffer(p->picture().data(), p->picture().size());
            artwork.size = p->picture().size();
            return artwork;
          }
        }
      }
    }
  
    // mpeg?
    {
      MPEG::File mpegfile(&fileStream, ID3v2::FrameFactory::instance());
      if (mpegfile.isValid() && mpegfile.ID3v2Tag()) {

        // log
        std::cout << "trying id3v2 picture\n";

        // get picture
        ID3v2::FrameList fl = mpegfile.ID3v2Tag()->frameListMap()[ID3TID_PICTURE];
        if (!fl.isEmpty()) {
          ID3v2::AttachedPictureFrame* p = static_cast<ID3v2::AttachedPictureFrame*>(fl.front());
          if (p != NULL) {
            std::cout << "got id3v2 picture\n";
            artwork.buffer = (unsigned char*) copy_buffer(p->picture().data(), p->picture().size());
            artwork.size = p->picture().size();
            return artwork;
          }
        }
      }
    }

  }

  // done
  return artwork;
}

void set_id3v2_artwork(ID3v2::Tag *id3v2tag, struct Artwork *artwork) {
  ID3v2::AttachedPictureFrame *frame = new ID3v2::AttachedPictureFrame();
  frame->setPicture(ByteVector((const char*) artwork->buffer, (unsigned int) artwork->size));
  id3v2tag->addFrame(frame);
}

FLAC::Picture* get_flac_picture(struct Artwork *artwork) {
  
  // make a copy
  char* copy = copy_buffer((const char*) artwork->buffer, artwork->size);

  // now create
  FLAC::Picture *picture = new FLAC::Picture();
  picture->setData(ByteVector(copy, (unsigned int) artwork->size));
  picture->setType(FLAC::Picture::FrontCover);
  
  // done
  return picture;

}

FFI_PLUGIN_EXPORT int set_artwork(const char* filename, struct Artwork *artwork) {

  // open a file stream so we can manage r/w access
  FileStream fileStream(filename, false);
  if (fileStream.isOpen() == false) {
    std::cout << "unable to open file\n";
    return 0;
  }

  //
  // mpeg as last because it will work with everything!
  //

  // mp4
  {
    MP4::File mp4file(&fileStream);
    if (mp4file.isValid() && mp4file.tag()) {

      // log
      std::cout << "deleting mp4 cover\n";

      // delete 1st
      mp4file.tag()->removeItem("covr");

      // write
      if (artwork->size > 0) {

        // do it
        std::cout << "adding mp4 cover\n";
        MP4::CoverArtList artList;
        MP4::CoverArt coverArt(MP4::CoverArt::Unknown, ByteVector((const char*) artwork->buffer, (unsigned int) artwork->size));
        artList.append(coverArt);
        mp4file.tag()->setItem("covr", MP4::Item(artList));
      
      }

      // done
      return mp4file.save();

    }
  }

  // flac
  {
    FLAC::File flacfile(&fileStream, ID3v2::FrameFactory::instance());
    if (flacfile.isValid()) {

      // log
      std::cout << "deleting flac pictures\n";

      // get tags
      ID3v2::Tag* id3v2tag = flacfile.ID3v2Tag();
      Ogg::XiphComment* xiphComments = flacfile.xiphComment();

      // 1st delete
      {
        std::cout << "deleting flac@flac pictures\n";
        flacfile.removePictures();
      }
      if (id3v2tag != NULL) {
        std::cout << "deleting id3v2@flac pictures\n";
        delete_id3v2_frame(id3v2tag, ID3TID_PICTURE);
      }
      if (xiphComments != NULL) {
        std::cout << "deleting ogg@flac pictures\n";
        xiphComments->removeAllPictures();
      }

      // write
      if (artwork->size > 0) {

        // update flac
        {
          std::cout << "adding flac@flac picture\n";
          flacfile.addPicture(get_flac_picture(artwork));
        }

        // update id3v2
        if (id3v2tag != NULL) {
          std::cout << "adding id3v2@flac picture\n";
          set_id3v2_artwork(id3v2tag, artwork);
        }

        // update ogg
        if (xiphComments != NULL) {
          std::cout << "adding ogg@flac picture\n";
          xiphComments->addPicture(get_flac_picture(artwork));
        }
        
      }

      // done
      std::cout << "here" << std::endl;
      return flacfile.save();

    }

  }

  // mpeg?
  {
    MPEG::File mpegfile(&fileStream, ID3v2::FrameFactory::instance());
    if (mpegfile.isValid() && mpegfile.ID3v2Tag()) {

      // log
      std::cout << "deleting id3v2 picture\n";

      // delete 1st
      delete_id3v2_frame(mpegfile.ID3v2Tag(), ID3TID_PICTURE);

      // write
      if (artwork->size > 0) {

        // do it
        std::cout << "adding id3v2 picture\n";
        set_id3v2_artwork(mpegfile.ID3v2Tag(), artwork);
      
      }

      // done
      return mpegfile.save();

    }
  }

  // too bad
  return 0;
  
}

FFI_PLUGIN_EXPORT void free_artwork(struct Artwork artwork) {
  SAFE_FREE(artwork.buffer);
}
