# encoding: utf-8

task default: [:cucumber, :quality]
task quality: [:rubocop, :reek, :flay, :flog]
task travis: [:quality]

Dir.glob('tasks/*.rake').each { |each| import each }
