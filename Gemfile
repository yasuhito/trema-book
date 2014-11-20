source 'https://rubygems.org'

gem 'trema', github: 'trema/trema', branch: 'develop'

group :development, :test do
  gem 'aruba', require: false
  gem 'cucumber', require: false
  gem 'rspec', require: false
  gem 'rspec-given', require: false

  gem 'guard', require: false
  gem 'guard-bundler', require: false
  gem 'guard-cucumber', require: false
  gem 'guard-rspec', require: false
  gem 'guard-rubocop', require: false

  gem 'flay', require: false
  gem 'flog', require: false
  gem 'reek', require: false
  gem 'rubocop', require: false
end

# The test group will be installed on Travis CI
group :test do
  gem 'codeclimate-test-reporter', require: false
  gem 'coveralls', require: false
end
