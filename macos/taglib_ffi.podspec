
Pod::Spec.new do |s|
  s.name             = 'taglib_ffi'
  s.version          = '1.0.0'
  s.summary          = 'TagLib FFI'
  s.description      = 'TagLib FFI for Dart/Flutter'
  s.homepage         = 'https://github.com/nbonamy/taglib_ffi'
  s.license          = { :file => '../LICENSE', :type => 'MIT' }
  s.author           = { 'Nicolas Bonamy' => 'nbonamy@gmail.com' }

  s.source           = { :git => 'https://github.com/nbonamy/taglib_ffi.git', :tag => s.version.to_s }
  s.source_files     = 'Classes/**/*'
  s.dependency 'FlutterMacOS'

  # s.dependency 'taglib-framework', '1.13'
  s.xcconfig = {
    'USER_HEADER_SEARCH_PATHS' => "#{ENV['HOMEBREW_PREFIX']}/include",
    'LIBRARY_SEARCH_PATHS' => "#{ENV['HOMEBREW_PREFIX']}/lib"
  }
  s.library = 'tag'

  s.platform = :osx, '11.0'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'

end
