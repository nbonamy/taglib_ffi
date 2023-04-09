# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(taglib_ffi_library VERSION 0.0.1 LANGUAGES CXX)

add_library(
  taglib_ffi SHARED "taglib_read_tags.cxx" "taglib_write_tags.cxx" "taglib_artwork.cxx"
)

set_target_properties(taglib_ffi PROPERTIES
  PUBLIC_HEADER taglib_ffi.h
  OUTPUT_NAME "taglib_ffi"
  LINKER_LANGUAGE CXX
)

target_compile_definitions(taglib_ffi PUBLIC DART_SHARED_LIB)

target_include_directories(taglib_ffi PUBLIC
  "$ENV{HOMEBREW_PREFIX}/include"
)

target_link_libraries(taglib_ffi PRIVATE "$ENV{HOMEBREW_PREFIX}/lib/libtag.dylib")
