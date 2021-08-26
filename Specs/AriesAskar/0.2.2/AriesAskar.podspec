Pod::Spec.new do |s|
    s.name                    = "AriesAskar"
    s.version                 = "0.2.2"
    s.summary                 = "Compiled static library + relevant headers for the Aries Askar library."
    s.description             = <<-DESC
                              An extended description of AriesAskar project.
                              DESC
    s.homepage                = "http://your.homepage/here"
    s.license                 = { :type => 'Copyright', :text => <<-LICENSE
                                  Copyright 2021
                                  Permission is granted to...
                                  LICENSE
                                }
    s.author                  = { "Svit Zebec" => "svit.zebec@kamino.si" }
    s.source                  = { :http => 'https://github.com/svitzebec/aries-askar/blob/main/Specs/AriesAskar/0.2.2/aries-askar.zip?raw=true' }
    s.ios.source_files        = 'aries-askar.h', 'AskarTypes.h'
    s.ios.vendored_libraries  = 'libaries_askar.a'
    s.ios.xcconfig            = { 'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}"' }
    s.platform = :ios
    s.ios.deployment_target   = '13.0'
end