require 'rake/clean'

CLOBBER << 'index.html'

task html: 'index.html'

# rubocop:disable LineLength
file 'index.html' => ['index.adoc', 'how_does_openflow_work.adoc', 'hello_trema.adoc'] do |t|
  sh "bundle exec asciidoctor -a icons=font -a toc=left -a source-highlighter=coderay -d book index.adoc --out-file #{t.name}"
end
# rubocop:enable LineLength
