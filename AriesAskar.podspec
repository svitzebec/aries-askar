Pod::Spec.new do |s|
    s.name         = "AriesAskar"
    s.version      = "0.2.2-test4"
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
    s.source       = { :http => '.' }
    s.pod_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
    s.user_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
    s.ios.source_files = 'askar_wallet.h', 'AriesAskar/AskarTypes.h'
    s.ios.vendored_libraries = 'libaries_askar.a'
    s.ios.xcconfig = { 'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}"' }
    s.platform = :ios
    s.swift_version = "5.0"
    s.ios.deployment_target  = '12.0'
end