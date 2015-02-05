task default: [:spec, :cucumber, :quality]
task quality: [:rubocop, :reek, :flog]
task travis: [:spec, :cucumber, :quality]

Dir.glob('tasks/*.rake').each { |each| import each }
