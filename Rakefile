require 'rake/clean'

CLEAN << 'book.xml'
CLOBBER << 'book.pdf'
CLOBBER << 'book.html'

task default: :render
task travis: :render

task render: ['book.html', 'book.pdf']

task html: 'book.html'

task :deploy do
  # fail if ENV['TRAVIS_BRANCH'] != 'develop'
  sh 'git checkout -B gh-pages'
  sh 'bundle exec rake html'
  sh 'git add -A .'
  sh %(git commit --quiet -m "Travis build #{ENV['TRAVIS_BUILD_NUMBER']}")
  sh %(git push --force --quiet "https://#{ENV['GH_TOKEN']}@#{ENV['GH_REF']}" gh-pages > /dev/null)
end

file 'book.pdf' => 'book.xml' do
  sh './vendor/asciidoctor-fopub/fopub book.xml -param body.font.family VL-PGothic-Regular -param dingbat.font.family VL-PGothic-Regular -param monospace.font.family VL-PGothic-Regular -param sans.font.family VL-PGothic-Regular -param title.font.family VL-PGothic-Regular'
end

file 'book.xml' => 'book.adoc' do
  sh 'bundle exec asciidoctor -b docbook -d book -a data-uri! book.adoc'
end

file 'book.html' => 'book.adoc' do |t|
  sh "bundle exec asciidoctor -d book book.adoc --out-file #{t.name}"
end

Dir.glob('tasks/*.rake').each { |each| import each }
