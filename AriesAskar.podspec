Pod::Spec.new do |s|
    s.name         = "AriesAskar"
    s.version      = "0.2.4"
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
    s.author             = { "AlonsoAGlobant" => "alonso.alvarez@globant.com" }
    s.source       = { :git => "git@github.com:AlonsoAGlobant/aries-askar.git", :tag => "#{s.version}" }
    s.vendored_frameworks = "AriesAskar.xcframework"
    s.platform = :ios
    s.swift_version = "5.0"
    s.ios.deployment_target  = '12.0'
end