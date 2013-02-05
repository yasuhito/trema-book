require "bindata"


class ChassisIdTlv < BinData::Primitive
  bit7 :tlv_type, :value => 1
  bit9 :tlv_info_length, :value => lambda { tlv_info_string.length }
  string :tlv_info_string, :read_length => :tlv_info_length


  def get
    self.tlv_info_string
  end


  def set value
    self.tlv_info_string = value
  end
end


class PortIdTlv < BinData::Primitive
  bit7 :tlv_type, :value => 2
  bit9 :tlv_info_length, :value => lambda { tlv_info_string.length }
  string :tlv_info_string, :read_length => :tlv_info_length


  def get
    self
  end


  def set value
    self.tlv_info_string = value
  end
end


class TtlTlv < BinData::Primitive
  bit7 :tlv_type, :value => 3
  bit9 :tlv_info_length, :value => 2
  string :ttl, :read_length => :tlv_info_length


  def get
    self
  end


  def set value
    self.ttl = BinData::Int16be.new( value ).to_binary_s
  end
end


class EndOfLldpu < BinData::Primitive
  bit7 :tlv_type, :value => 0
  bit9 :tlv_info_length, :value => 0
  string :ttl, :value => ""


  def get
    self
  end


  def set value
    # Read Only
  end
end


class LldpFrame < BinData::Record
  endian :big

  uint48 :destination_mac, :value => 0x0180c200000e
  uint48 :source_mac
  uint16 :ethertype, :value => 0x88cc
  chassis_id_tlv :chassis_id
  port_id_tlv :port_id
  ttl_tlv :ttl
  end_of_lldpu
end


class Lldp
  def self.read packet_in
    lldp_frame = LldpFrame.read( packet_in.data )
    new lldp_frame.source_mac, lldp_frame.chassis_id.unpack( "Q*" )[ 0 ]
  end


  def initialize mac, dpid
    @frame = LldpFrame.new
    @frame.source_mac = mac.value
    @frame.chassis_id = BinData::Uint64le.new( dpid ).to_binary_s
    @frame.port_id = "\x01Port aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    @frame.ttl = 120
  end


  def dpid
    @frame.chassis_id.unpack( "Q*" )[ 0 ]
  end


  def source_mac
    Trema::Mac.new @frame.source_mac.value
  end


  def to_binary
    @frame.to_binary_s
  end
end


module Trema
  class PacketIn
    def lldp?
      eth_type == 0x88CC
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
