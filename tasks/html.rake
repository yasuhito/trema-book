require 'rake/clean'

CLOBBER << 'index.html'

task html: 'index.html'

file 'index.html' => 'book.adoc' do |t|
  sh "bundle exec asciidoctor -d book book.adoc --out-file #{t.name}"
end
