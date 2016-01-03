require 'rake/clean'

CLOBBER << 'index.html'

task :html do
  Rake::Task['index.html'].invoke
end

file 'index.html' => ADOC do |t|
  revnumber = `git describe --abbrev=0`.chomp
  sh 'bundle exec asciidoctor '\
     "-a revnumber=#{revnumber} "\
     '-a icons=font '\
     '-a toc=left '\
     '-a source-highlighter=coderay '\
     '-a experimental '\
     "-d book index.adoc --out-file #{t.name}"
end
