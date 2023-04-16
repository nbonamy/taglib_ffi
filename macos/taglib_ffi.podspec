
Pod::Spec.new do |s|
  s.name             = 'taglib_ffi'
  s.version          = '1.0.0'
  s.summary          = 'TagLib FFI'
  s.description      = 'TagLib FFI'
  s.homepage         = 'https://github.com/nbonamy/taglib_ffi'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Nicolas Bonamy' => 'nbonamy@gmail.com' }

  # This will ensure the source files in Classes/ are included in the native
  # builds of apps using this FFI plugin. Podspec does not support relative
  # paths, so Classes contains a forwarder C file that relatively imports
  # `../src/*` so that the C sources can be shared among all target platforms.
  s.source           = { :git => 'https://github.com/nbonamy/taglib_ffi.git', :tag => 'main' }
  s.source_files     = 'Classes/**/*'
  s.dependency 'FlutterMacOS'

  s.dependency 'taglib.framework', '1.0.0'

  s.platform = :osx, '11.0'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
