
Pod::Spec.new do |s|
  s.name             = 'taglib.framework'
  s.version          = '1.13'
  s.summary          = 'TagLib'
  s.description      = 'TagLib'
  s.homepage         = 'https://taglib.org/'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Nicolas Bonamy' => 'nbonamy@gmail.com' }
  s.source           = { :git => 'https://github.com/nbonamy/taglib_ffi.git', :tag => 'main' }
  s.module_name      = 'taglib.framework'
  s.vendored_frameworks = 'tag.xcframework'
  s.platform = :osx, '11.0'
end
