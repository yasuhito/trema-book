require "bindata"


class ChassisIdTlv < BinData::Primitive
  bit7 :tlv_type, :value => 1
  bit9 :tlv_info_length, :value => lambda { tlv_info_string.length }
  string :tlv_info_string, :read_length => :tlv_info_length


  def get
    self
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


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
