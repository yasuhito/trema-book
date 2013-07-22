require File.join( File.dirname( __FILE__ ), "spec_helper" )
require "port-id-tlv"


describe PortIdTlv do
  subject { PortIdTlv.read( data.pack( "C*" ) ) }

  context "parsing a raw data" do
    let( :data ) { [ 0x04, 0x0d, 0x01, 0x55, 0x70, 0x6c, 0x69, 0x6e, 0x6b, 0x20, 0x74, 0x6f, 0x20, 0x53, 0x31 ] }

    its( :tlv_type ) { should eq 2 }
    its( :tlv_info_length ) { should eq 13 }
    its( :subtype ) { should eq 1 }
    its( :port_id ) { should eq data[ -12..-1 ].pack( "C*" ) }
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
