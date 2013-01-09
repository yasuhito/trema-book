require "bundler"

begin
  Bundler.setup :default, :development
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end


# $LOAD_PATH.unshift( File.expand_path( File.dirname( __FILE__ ) + "/../../ruby" ) )


require "aruba/cucumber"
require "rspec"
require "trema"


def ps_entry_of name
  `ps -ef | grep -w "#{ name } " | grep -v grep`
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
