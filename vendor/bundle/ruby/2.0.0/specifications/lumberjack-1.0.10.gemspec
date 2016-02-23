# -*- encoding: utf-8 -*-
# stub: lumberjack 1.0.10 ruby lib

Gem::Specification.new do |s|
  s.name = "lumberjack"
  s.version = "1.0.10"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Brian Durand"]
  s.date = "2016-01-02"
  s.description = "A simple, powerful, and very fast logging utility that can be a drop in replacement for Logger or ActiveSupport::BufferedLogger. Provides support for automatically rolling log files even with multiple processes writing the same log file."
  s.email = ["bbdurand@gmail.com"]
  s.extra_rdoc_files = ["README.rdoc"]
  s.files = ["README.rdoc"]
  s.homepage = "http://github.com/bdurand/lumberjack"
  s.licenses = ["MIT"]
  s.rdoc_options = ["--charset=UTF-8", "--main", "README.rdoc"]
  s.rubygems_version = "2.4.5"
  s.summary = "A simple, powerful, and very fast logging utility that can be a drop in replacement for Logger or ActiveSupport::BufferedLogger."

  s.installed_by_version = "2.4.5" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<rspec>, [">= 3.0"])
      s.add_development_dependency(%q<timecop>, [">= 0.8.0"])
    else
      s.add_dependency(%q<rspec>, [">= 3.0"])
      s.add_dependency(%q<timecop>, [">= 0.8.0"])
    end
  else
    s.add_dependency(%q<rspec>, [">= 3.0"])
    s.add_dependency(%q<timecop>, [">= 0.8.0"])
  end
end
