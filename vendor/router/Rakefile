task default: [:cucumber, :quality]
task quality: [:rubocop, :reek, :flog, :flay]
task travis: [:quality]

Dir.glob('tasks/*.rake').each { |each| import each }
