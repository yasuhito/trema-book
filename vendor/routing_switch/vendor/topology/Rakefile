task default: [:spec, :cucumber, :quality]
task quality: [:rubocop, :reek, :flog, :flay]
task travis: [:spec, :quality]

task :run do
  sh 'trema run ./topology-controller.rb -c triangle.conf'
end

Dir.glob('tasks/*.rake').each { |each| import each }
