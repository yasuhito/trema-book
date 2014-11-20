task default: 'vendor:cbench'
task travis: [:default, :spec]

Dir.glob('tasks/*.rake').each { |each| import each }
