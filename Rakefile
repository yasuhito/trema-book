task default: [:spec, :cucumber, :quality]
task quality: [:rubocop, :reek, :flog]
task travis: [:spec, :quality]

Dir.glob('tasks/*.rake').each { |each| import each }
