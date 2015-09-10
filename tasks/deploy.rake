# coding: utf-8
# rubocop:disable LineLength
task :deploy do
  if ENV['TRAVIS_BRANCH'] != 'master'
    sh 'git checkout -B preview-html'
    sh 'bundle exec rake index.html'
    sh "mv index.html #{ENV['TRAVIS_JOB_NUMBER']}.html"
    sh 'git pull'
    sh "git add -A -f #{ENV['TRAVIS_JOB_NUMBER']}.html"
    sh %(git commit -m "Preview HTML #{ENV['TRAVIS_JOB_NUMBER']}")
    sh %(git push --quiet --force "https://#{ENV['GH_TOKEN']}@github.com/yasuhito/trema-book.git" preview-html > /dev/null), verbose: false
    sh %(curl --data-urlencode "message=[#{ENV['TRAVIS_JOB_NUMBER']}.htmlをプレビュー](http://htmlpreview.github.io/?https://github.com/yasuhito/trema-book/blob/preview-html/#{ENV['TRAVIS_JOB_NUMBER']}.html)" https://webhooks.gitter.im/e/#{ENV['GITTER_TOKEN']}), verbose: false
  else
    sh 'rm .gitignore'
    sh 'git checkout -B gh-pages'
    sh 'bundle exec rake index.html'
    sh 'git add -A .'
    sh %(git commit --quiet -m "Travis build #{ENV['TRAVIS_BUILD_NUMBER']}")
    sh %(git push --force --quiet "https://#{ENV['GH_TOKEN']}@github.com/yasuhito/trema-book.git" gh-pages > /dev/null), verbose: false
  end
  # rubocop:enable LineLength
end
