begin
  require 'reek/rake/task'
  Reek::Rake::Task.new do |t|
    t.fail_on_error = false
    t.verbose = false
    t.reek_opts = '--quiet'
    t.source_files = FileList['lib/**/*.rb']
  end
rescue LoadError
  task :reek do
    $stderr.puts 'reek is disabled'
  end
end
