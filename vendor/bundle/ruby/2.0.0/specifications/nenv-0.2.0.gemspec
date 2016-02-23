# -*- encoding: utf-8 -*-
# stub: nenv 0.2.0 ruby lib

Gem::Specification.new do |s|
  s.name = "nenv"
  s.version = "0.2.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Cezary Baginski"]
  s.date = "2015-01-16"
  s.description = "Using ENV is like using raw SQL statements in your code. We all know how that ends..."
  s.email = ["cezary@chronomantic.net"]
  s.homepage = "https://github.com/e2/nenv"
  s.licenses = ["MIT"]
  s.rubygems_version = "2.4.5"
  s.summary = "Convenience wrapper for Ruby's ENV"

  s.installed_by_version = "2.4.5" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<bundler>, ["~> 1.7"])
      s.add_development_dependency(%q<rspec>, ["~> 3.1"])
      s.add_development_dependency(%q<rake>, ["~> 10.0"])
    else
      s.add_dependency(%q<bundler>, ["~> 1.7"])
      s.add_dependency(%q<rspec>, ["~> 3.1"])
      s.add_dependency(%q<rake>, ["~> 10.0"])
    end
  else
    s.add_dependency(%q<bundler>, ["~> 1.7"])
    s.add_dependency(%q<rspec>, ["~> 3.1"])
    s.add_dependency(%q<rake>, ["~> 10.0"])
  end
end
