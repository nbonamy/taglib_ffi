
Pod::Spec.new do |s|
  s.name             = 'taglib-framework'
  s.version          = '1.0.0'
  s.summary          = 'TagLib'
  s.description      = 'TagLib as a universal framwork'
  s.homepage         = 'https://taglib.org/'
  s.license          = { :file => '../taglib/LICENSE', :type => 'MPL' }
  s.author           = { 'Nicolas Bonamy' => 'nbonamy@gmail.com' }
  s.source           = { :git => 'https://github.com/nbonamy/taglib_ffi.git', :tag => '1.13' }
  s.module_name      = 'taglib'
  s.vendored_frameworks = 'macos/tag.framework'
  s.platform = :osx, '11.0'
end
