# -*- encoding: utf-8 -*-
# stub: guard-rake 1.0.0 ruby lib

Gem::Specification.new do |s|
  s.name = "guard-rake"
  s.version = "1.0.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Scott Barron"]
  s.date = "2014-11-06"
  s.description = "guard-rake automatically runs Rake tasks from your Rakefile"
  s.email = ["scott@elitists.net"]
  s.homepage = "http://github.com/rubyist/guard-rake"
  s.rubygems_version = "2.4.5"
  s.summary = "Guard for running rake tasks"

  s.installed_by_version = "2.4.5" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<guard>, [">= 0"])
      s.add_runtime_dependency(%q<rake>, [">= 0"])
    else
      s.add_dependency(%q<guard>, [">= 0"])
      s.add_dependency(%q<rake>, [">= 0"])
    end
  else
    s.add_dependency(%q<guard>, [">= 0"])
    s.add_dependency(%q<rake>, [">= 0"])
  end
end
