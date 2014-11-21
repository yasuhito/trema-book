task default: 'vendor:cbench'
task travis: ['vendor:cbench', :spec, :cucumber]

Dir.glob('tasks/*.rake').each { |each| import each }
