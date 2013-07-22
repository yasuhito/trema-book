require "rubygems"

require "bindata"
require "lldp-frame"


class Lldp
  def self.read raw_data
    lldp_frame = LldpFrame.read( raw_data )
    new lldp_frame.dpid, lldp_frame.port_id.unpack( "S" )[ 0 ]
  end


  def initialize dpid, port_number, destination_mac = 0x0180c200000e
    @frame = LldpFrame.new
    @frame.destination_mac = destination_mac
    @frame.chassis_id.subtype = 7
    @frame.chassis_id = BinData::Uint64be.new( dpid ).to_binary_s.unpack( "C8" )[ 2..7 ].pack( "C6" )
    @frame.port_id.subtype = 7
    @frame.port_id = BinData::Uint16le.new( port_number ).to_binary_s
    @frame.ttl = 120
  end


  def dpid
    @frame.dpid
  end


  def optional_tlv
    @frame.optional_tlv
  end


  def port_number
    @frame.port_id.unpack( "S" )[ 0 ]
  end


  def to_binary
    @frame.to_binary_s + "\000" * ( 64 - @frame.num_bytes )
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
