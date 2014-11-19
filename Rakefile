task travis: [:vendor_hello_trema, :vendor_learning_switch,
              :prepare, :master, :rubocop]

Dir.glob('tasks/*.rake').each { |each| import each }
