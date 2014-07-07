# encoding: utf-8

require 'codeclimate-test-reporter'
CodeClimate::TestReporter.start

require 'rspec'

# Requires supporting files with custom matchers and macros, etc,
# in ./support/ and its subdirectories.
Dir["#{File.dirname(__FILE__)}/support/**/*.rb"].each do |each|
  require File.expand_path(each)
end
