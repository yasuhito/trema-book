task default: [:spec, :cucumber, :quality]
task quality: [:rubocop, :reek, :flog, :flay]
task travis: [:spec, :quality]

Dir.glob('tasks/*.rake').each { |each| import each }
