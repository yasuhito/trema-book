task default: [:cucumber, :quality]
task quality: [:rubocop, :reek, :flog, :flay]

Dir.glob('tasks/*.rake').each { |each| import each }
