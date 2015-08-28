require 'rake/clean'

CLEAN << 'book.xml'
CLOBBER << 'book.pdf'

task default: :render
task travis: :render

task render: 'book.pdf'

file 'book.pdf' => 'book.xml' do
  sh './vendor/asciidoctor-fopub/fopub book.xml -param body.font.family VL-PGothic-Regular -param dingbat.font.family VL-PGothic-Regular -param monospace.font.family VL-PGothic-Regular -param sans.font.family VL-PGothic-Regular -param title.font.family VL-PGothic-Regular'
end

file 'book.xml' => 'book.adoc' do
  sh 'bundle exec asciidoctor -b docbook -d book -a data-uri! book.adoc'
end

Dir.glob('tasks/*.rake').each { |each| import each }
