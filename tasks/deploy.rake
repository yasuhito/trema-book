# rubocop:disable LineLength
task :deploy do
  if ENV['TRAVIS_BRANCH'] != 'develop'
    fail 'This is not a develop branch. No deployment will be done.'
  end
  if ENV['TRAVIS_PULL_REQUEST'] != 'false'
    fail 'This is a pull request. No deployment will be done.'
  end
  sh 'git checkout -B gh-pages'
  sh 'bundle exec rake html'
  sh 'git add -A .'
  sh %(git commit --quiet -m "Travis build #{ENV['TRAVIS_BUILD_NUMBER']}")
  sh %(git push --force --quiet "https://#{ENV['GH_TOKEN']}@github.com/yasuhito/trema-book.git" gh-pages > /dev/null), verbose: false
end
# rubocop:enable LineLength
