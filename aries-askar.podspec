Pod::Spec.new do |spec|

  # ―――  Spec Metadata  ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  These will help people to find your library, and whilst it
  #  can feel like a chore to fill in it's definitely to your advantage. The
  #  summary should be tweet-length, and the description more in depth.
  #

  spec.name         = "aries-askar"
  spec.version      = "0.2.3"
  spec.summary      = "A short description of Aries Askar."

  spec.description  = "A Description for Aries Askar"

  spec.homepage     = "http://kamino.si"

  spec.license      = { :type => "Apache License, Version 2.0", :file => "LICENSE" }
  spec.author       = "Kamino"
  spec.platform     = :ios

  spec.ios.deployment_target = "12.0"

  spec.ios.vendored_libraries = "libaries_askar.a"

  spec.source       = { :git => "https://github.com/AlonsoAGlobant/aries-askar.git", :tag => "#{spec.version}" }

end