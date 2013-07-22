require File.join( File.dirname( __FILE__ ), "spec_helper" )
require "chassis-id-tlv"


describe ChassisIdTlv do
  subject { ChassisIdTlv.read( data.pack( "C*" ) ) }

  context "parsing a raw data" do
    let( :data ) { [ 0x02, 0x07, 0x04, 0x00, 0x19, 0x2f, 0xa7, 0xb2, 0x8d ] }

    its( :tlv_type ) { should eq 1 }
    its( :tlv_info_length ) { should eq 7 }
    its( :subtype ) { should eq 4 }
    its( :chassis_id ) { should eq data[ -6..-1 ].pack( "C*" ) }
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
