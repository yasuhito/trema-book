# -*- coding: utf-8 -*-
require 'rubygems'

require 'rspec'
require 'rspec/autorun'

# Requires supporting files with custom matchers and macros, etc,
# in ./support/ and its subdirectories.
Dir["#{ File.dirname(__FILE__) }/support/**/*.rb"].each do | each |
  require File.expand_path(each)
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
