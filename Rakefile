task default: [:test, :quality]
task test: [:spec, :cucumber]
task quality: [:reek, :flog, :flay, :rubocop]
task travis: [:spec, :quality]

Dir.glob('tasks/*.rake').each { |each| import each }
