task default: [:rubocop, :render]
task travis: [:rubocop, :render]
task render: [:html, :pdf]

ADOC = Dir.glob('*.adoc')

Dir.glob('tasks/*.rake').each { |each| import each }
