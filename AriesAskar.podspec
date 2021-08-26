Pod::Spec.new do |s|
    s.name         = "AriesAskar"
    s.version      = "0.2.2-test"
    s.summary      = "A brief description of AriesAskar project."
    s.description  = <<-DESC
    An extended description of AriesAskar project.
    DESC
    s.homepage     = "http://your.homepage/here"
    s.license = { :type => 'Copyright', :text => <<-LICENSE
                   Copyright 2021
                   Permission is granted to...
                  LICENSE
                }
    s.author             = { "Svit Zebec" => "svit.zebec@kamino.si" }
    s.source       = { :http => 'https://github.com/svitzebec/aries-askar/blob/feature/static-lib-ios-pod/builds/ios/v0.2.2/aries-askar.zip' }
    s.vendored_libraries = 'libaries_askar.a'
    s.ios.xcconfig = { "HEADER_SEARCH_PATHS" => "$(PODS_ROOT)" }
    s.platform = :ios
    s.swift_version = "5.0"
    s.ios.deployment_target  = '12.0'
end