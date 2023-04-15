

char* copy_taglib_str(const TagLib::String &str);
char* copy_buffer(const char *src, size_t length);

void delete_id3v2_frame(TagLib::ID3v2::Tag* id3v2tag, const char *frame_name);

TagLib::String get_xiph_comment(TagLib::Ogg::XiphComment *xiphComments, const char *key);

