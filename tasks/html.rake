require 'rake/clean'

CLOBBER << 'index.html'

file 'index.html' => ADOC do |t|
  sh 'bundle exec asciidoctor '\
     '-a icons=font '\
     '-a toc=left '\
     '-a source-highlighter=coderay '\
     "-d book index.adoc --out-file #{t.name}"
end
