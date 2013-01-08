require "rspec/core"
require "rspec/core/rake_task"


task :default => :spec


RSpec::Core::RakeTask.new( :spec ) do | spec |
  spec.pattern = FileList[ "spec/**/*_spec.rb" ]
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
