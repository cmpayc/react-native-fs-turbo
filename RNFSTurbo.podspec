require "json"

package = JSON.parse(File.read(File.join(__dir__, "./package.json")))

Pod::Spec.new do |s|
  s.name            = "RNFSTurbo"
  s.version         = package["version"]
  s.summary         = package["description"]
  s.description     = package["description"]
  s.homepage        = package["homepage"]
  s.license         = package["license"]
  s.platforms       = { :ios => "12.4" }
  s.author          = package["author"]
  s.source          = { :git => "https://github.com/cmpayc/react-native-fs-turbo.git", :tag => "#{s.version}" }
  s.source_files = [
    "ios/**/*.{h,m,mm}",
    "cpp/**/*.{hpp,cpp,c,h}",
  ]
  s.resource_bundles = { 'RNFSTurbo_PrivacyInfo' => 'ios/PrivacyInfo.xcprivacy' }
  s.compiler_flags = '-x objective-c++ -DUSE_ARM_AES'
  s.frameworks = 'Photos', 'AVFoundation'
  s.pod_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++20"
  }
  install_modules_dependencies(s)
end
