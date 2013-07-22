require File.join( File.dirname( __FILE__ ), "spec_helper" )
require "ttl-tlv"


describe TtlTlv do
  subject { TtlTlv.read( data.pack( "C*" ) ) }

  context "parsing a raw data" do
    let( :data ) { [ 0x06, 0x02, 0x00, 0x78 ] }

    its( :tlv_type ) { should eq 3 }
    its( :tlv_info_length ) { should eq 2 }
    its( :ttl ) { should eq data[ -2..-1 ].pack( "C*" ) }
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
