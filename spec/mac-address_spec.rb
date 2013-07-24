require File.join( File.dirname( __FILE__ ), "spec_helper" )
require "mac-address"


describe MacAddress do
  subject { MacAddress.new( "01:80:c2:00:00:0e" ) }

  its( :to_binary_s ) { should eq "\001\200\302\000\000\016" }
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
