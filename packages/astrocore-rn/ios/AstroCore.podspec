require 'json'

package = JSON.parse(File.read(File.join(__dir__, '..', 'package.json')))

Pod::Spec.new do |s|
  s.name             = 'AstroCore'
  s.version          = package['version'] || '0.1.0'
  s.summary          = 'React Native JSI bindings for the AstroCore C++ engine.'
  s.license          = { :type => 'MIT' }
  s.homepage         = package['homepage'] || 'https://example.com/astrocore'
  s.source           = { :path => '.' }
  s.author           = package['author'] || { 'AstroCore' => 'dev@example.com' }
  s.platforms        = { :ios => '13.0' }
  s.static_framework = true
  s.requires_arc     = true

  s.source_files = [
    'ios/**/*.{h,mm}',
    'cpp/src/**/*.{cpp,hpp}',
    'cpp/include/**/*.hpp'
  ]

  s.header_mappings_dir = 'cpp/include'
  s.preserve_paths = 'cpp'
  s.pod_target_xcconfig = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++20',
    'CLANG_CXX_LIBRARY' => 'libc++',
    'HEADER_SEARCH_PATHS' => "\"$(PODS_TARGET_SRCROOT)/cpp/include\""
  }

  s.dependency 'React-Core'
  s.dependency 'React-jsi'
end
