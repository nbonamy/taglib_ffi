#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint taglib_ffi.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'taglib_ffi'
  s.version          = '0.0.1'
  s.summary          = 'TagLib FFI'
  s.description      = 'TagLib FFI'
  s.homepage         = 'https://github.com/nbonamy/taglib_ffi'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'nbonamy@gmail.com' }

  # This will ensure the source files in Classes/ are included in the native
  # builds of apps using this FFI plugin. Podspec does not support relative
  # paths, so Classes contains a forwarder C file that relatively imports
  # `../src/*` so that the C sources can be shared among all target platforms.
  s.source           = { :path => '.' }
  s.source_files     = 'Classes/**/*'
  s.xcconfig = {
    'USER_HEADER_SEARCH_PATHS' => '/usr/local/include',
    'LIBRARY_SEARCH_PATHS' => '/usr/local/lib'
  }
  s.libraries = 'tag'
  s.dependency 'FlutterMacOS'

  s.platform = :osx, '13.0'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
