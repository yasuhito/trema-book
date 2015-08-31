task default: [:rubocop, :render]
task travis: [:rubocop, :render]

task render: [:html, :pdf]

Dir.glob('tasks/*.rake').each { |each| import each }
