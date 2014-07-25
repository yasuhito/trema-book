# encoding: utf-8

task default: [:quality]
task quality: [:rubocop]
task travis: [:quality]

Dir.glob('tasks/*.rake').each { |each| import each }
