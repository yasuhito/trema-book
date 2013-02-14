require File.join( File.dirname( __FILE__ ), "spec_helper" )
require "lldp-frame"


describe LldpFrame do
  it "should create a valid LLDP packet" do
    # sample packet taken from http://www.cloudshark.org/captures/05a981251df9
    sample_packet = [
      0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e,  # Destination MAC
      0x00, 0x19, 0x2f, 0xa7, 0xb2, 0x8d,  # Source MAC
      0x88, 0xcc,  # Ethertype
      0x02, 0x07, 0x04, 0x00, 0x19, 0x2f, 0xa7, 0xb2, 0x8d,  # Chassis ID TLV
      0x04, 0x0d, 0x01, 0x55, 0x70, 0x6c, 0x69, 0x6e, 0x6b, 0x20, 0x74, 0x6f, 0x20, 0x53, 0x31,  # Port ID TLV
      0x06, 0x02, 0x00, 0x78,  # Time to live TLV
      0x00, 0x00  # End of LLDPDU TLV
    ].pack( "C*" )

    lldp_frame = LldpFrame.new
    lldp_frame.destination_mac = 0x0180c200000e
    lldp_frame.source_mac = 0x00192fa7b28d
    lldp_frame.chassis_id.subtype = 4
    lldp_frame.chassis_id = "\x00\x19\x2f\xa7\xb2\x8d"
    lldp_frame.port_id.subtype = 1
    lldp_frame.port_id = "Uplink to S1"
    lldp_frame.ttl = 120

    lldp_frame.to_binary_s.unpack( "H*" ).should == sample_packet.unpack( "H*" )
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
