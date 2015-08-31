require 'rake/clean'

CLEAN << 'book.xml'
CLOBBER << 'book.pdf'

file 'book.pdf' => 'book.xml' do
  fopub_options = %w(-param body.font.family VL-PGothic-Regular
                     -param dingbat.font.family VL-PGothic-Regular
                     -param monospace.font.family VL-PGothic-Regular
                     -param sans.font.family VL-PGothic-Regular
                     -param title.font.family VL-PGothic-Regular)
  sh "./vendor/asciidoctor-fopub/fopub book.xml #{fopub_options.join(' ')}"
end

file 'book.xml' => ADOC do
  sh 'bundle exec asciidoctor -b docbook -d book -a data-uri! book.adoc'
end
