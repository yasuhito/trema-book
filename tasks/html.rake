require 'rake/clean'

CLOBBER << 'index.html'

task html: 'index.html'

# rubocop:disable LineLength
file 'index.html' => ['index.adoc', 'how_does_openflow_work.adoc', 'hello_trema.adoc'] do |t|
  revnumber = `git describe --abbrev=0`.chomp
  sh %W(bundle exec asciidoctor
        -a revnumber=#{revnumber}
        -a icons=font
        -a toc=left
        -a source-highlighter=coderay
        -d book index.adoc --out-file #{t.name}).join(' ')
end
# rubocop:enable LineLength
