require File.join( File.dirname( __FILE__ ), "spec_helper" )
require "end-of-lldpdu-value"


describe EndOfLldpduValue do
  subject { EndOfLldpduValue.new }

  its( :tlv_info_string ) { should be_empty }
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
