# -*- encoding: utf-8 -*-
# stub: shellany 0.0.1 ruby lib

Gem::Specification.new do |s|
  s.name = "shellany"
  s.version = "0.0.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Cezary Baginski"]
  s.date = "2014-12-25"
  s.description = "MRI+JRuby compatible command output capturing"
  s.email = ["cezary@chronomantic.net"]
  s.homepage = ""
  s.licenses = ["MIT"]
  s.rubygems_version = "2.4.5"
  s.summary = "Simple, somewhat portable command capturing"

  s.installed_by_version = "2.4.5" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<bundler>, ["~> 1.7"])
    else
      s.add_dependency(%q<bundler>, ["~> 1.7"])
    end
  else
    s.add_dependency(%q<bundler>, ["~> 1.7"])
  end
end
