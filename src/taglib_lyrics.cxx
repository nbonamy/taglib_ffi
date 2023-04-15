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
#include "taglib/unsynchronizedlyricsframe.h"
#include "taglib/mp4tag.h"
#include "taglib/tpropertymap.h"
#include "taglib/xiphcomment.h"
#include "taglib/attachedpictureframe.h"

#include "id3_tags.h"

#include "taglib_utils.h"

using namespace TagLib;

bool get_id3v2_lyrics(ID3v2::Tag *id3v2tag, struct Lyrics *lyrics) {
  ID3v2::FrameList fl = id3v2tag->frameListMap()[ID3TID_UNSYNCEDLYRICS];
  if (!fl.isEmpty()) {
    ID3v2::UnsynchronizedLyricsFrame* f = static_cast<ID3v2::UnsynchronizedLyricsFrame*>(fl.front());
    if (f != NULL) {
      lyrics->lyrics = copy_taglib_str(f->text());
      return true;
    }
  }
  return false;
}

FFI_PLUGIN_EXPORT struct Lyrics get_lyrics(const char* filename) {

  // init
  struct Lyrics lyrics;
  memset(&lyrics, 0, sizeof(lyrics));
  
  // open a file stream so we can manage r/w access
  FileStream fileStream(filename, true);
  if (fileStream.isOpen() == false) {
    //std::cout << "unable to open file\n";
  }

  // mp4
  {
    MP4::File mp4file(&fileStream);
    if (mp4file.isValid() && mp4file.tag()) {

      // log
      //std::cout << "trying mp4 lyrics\n";

      // do it
      if (mp4file.tag()->contains("\251lyr")) {
        MP4::Item item = mp4file.tag()->item("\251lyr");
        //std::cout << "got mp4 lyrics\n";
        lyrics.lyrics = copy_taglib_str(item.toStringList().toString());
        return lyrics;
      }
    }
  }

  // flac
  {
    FLAC::File flacfile(&fileStream, ID3v2::FrameFactory::instance());
    if (flacfile.isValid()) {

      // log
      //std::cout << "trying flac lyrics\n";

      // vorbis
      Ogg::XiphComment* xiphComments = flacfile.xiphComment();
      if (xiphComments != NULL) {
        if (xiphComments->contains("LYRICS")) {
          //std::cout << "got ogg@flac lyrics\n";
          lyrics.lyrics = copy_taglib_str(get_xiph_comment(xiphComments, "LYRICS"));
          return lyrics;
        }
      }

      // then id3v2
      if (flacfile.ID3v2Tag()) {
        if (get_id3v2_lyrics(flacfile.ID3v2Tag(), &lyrics)) {
          //std::cout << "got id3v2@flac lyrics\n";
          return lyrics;
        }
      }
    }
  
    // mpeg?
    {
      MPEG::File mpegfile(&fileStream, ID3v2::FrameFactory::instance());
      if (mpegfile.isValid() && mpegfile.ID3v2Tag()) {

        // log
        //std::cout << "trying id3v2 lyrics\n";

        // get lyrics
        if (get_id3v2_lyrics(mpegfile.ID3v2Tag(), &lyrics)) {
          //std::cout << "got id3v2@flac lyrics\n";
          return lyrics;
        }
      }
    }

  }

  // done
  return lyrics;
}

void set_id3v2_lyrics(ID3v2::Tag *id3v2tag, struct Lyrics *lyrics) {
  ID3v2::UnsynchronizedLyricsFrame *frame = new ID3v2::UnsynchronizedLyricsFrame();
  frame->setText(String(lyrics->lyrics, String::UTF8));
  id3v2tag->addFrame(frame);
}

FFI_PLUGIN_EXPORT int set_lyrics(const char* filename, struct Lyrics *lyrics) {

  // open a file stream so we can manage r/w access
  FileStream fileStream(filename, false);
  if (fileStream.isOpen() == false) {
    //std::cout << "unable to open file\n";
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
      //std::cout << "deleting mp4 lyrics\n";

      // delete 1st
      mp4file.tag()->removeItem("\251lyr");

      // write
      if (strlen(lyrics->lyrics) > 0) {
        //std::cout << "adding mp4 lyrics\n";
        mp4file.tag()->setItem("\251lyr", MP4::Item(String(lyrics->lyrics, String::UTF8)));
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
      //std::cout << "deleting flac lyrics\n";

      // get tags
      ID3v2::Tag* id3v2tag = flacfile.ID3v2Tag();
      Ogg::XiphComment* xiphComments = flacfile.xiphComment();

      // delete
      if (id3v2tag != NULL) {
        //std::cout << "deleting id3v2@flac lyrics\n";
        delete_id3v2_frame(id3v2tag, ID3TID_UNSYNCEDLYRICS);
      }
      if (xiphComments != NULL) {
        //std::cout << "deleting ogg@flac lyrics\n";
        xiphComments->removeFields("LYRICS");
      }

      // write
      if (strlen(lyrics->lyrics) > 0) {

        // update id3v2
        if (id3v2tag != NULL) {
          //std::cout << "adding id3v2@flac lyrics;\n";
          set_id3v2_lyrics(id3v2tag, lyrics);
        }

        // update ogg
        if (xiphComments != NULL) {
          //std::cout << "adding ogg@flac lyrics\n";
          xiphComments->addField("LYRICS", String(lyrics->lyrics, String::UTF8));
        }
        
      }

      // done
      //std::cout << "here" << std::endl;
      return flacfile.save();

    }

  }

  // mpeg?
  {
    MPEG::File mpegfile(&fileStream, ID3v2::FrameFactory::instance());
    if (mpegfile.isValid() && mpegfile.ID3v2Tag()) {

      // log
      //std::cout << "deleting id3v2 lyrics\n";

      // delete 1st
      delete_id3v2_frame(mpegfile.ID3v2Tag(), ID3TID_UNSYNCEDLYRICS);

      // update
      if (strlen(lyrics->lyrics) > 0) {
        //std::cout << "adding id3v2 lyrics\n";
        set_id3v2_lyrics(mpegfile.ID3v2Tag(), lyrics);
      }

      // done
      return mpegfile.save();

    }
  }

  // too bad
  return 0;
  
}

FFI_PLUGIN_EXPORT void free_lyrics(struct Lyrics lyrics) {
  SAFE_FREE(lyrics.lyrics);
}
