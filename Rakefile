require 'cucumber/rake/task'
require 'rspec/core'
require 'rspec/core/rake_task'

task :default => [:spec, :cucumber, :quality]
task :quality => [:rubocop, :reek]
task :travis => :quality

task :run do
  sh 'trema run ./topology-controller.rb -c triangle.conf'
end

RSpec::Core::RakeTask.new(:spec) do |spec|
  spec.pattern = FileList['spec/**/*_spec.rb']
  spec.rspec_opts = '--format documentation --color'
end

Cucumber::Rake::Task.new do |t|
  t.cucumber_opts = 'features --tags ~@wip'
end

require 'rubocop/rake_task'
Rubocop::RakeTask.new

require 'reek/rake/task'
Reek::Rake::Task.new do |t|
  t.fail_on_error = false
  t.verbose = false
  t.ruby_opts = ['-rubygems']
  t.reek_opts = '--quiet'
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
