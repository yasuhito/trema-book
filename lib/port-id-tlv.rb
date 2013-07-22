require "rubygems"
require "bindata"


class PortIdTlv < BinData::Primitive
  endian :big

  bit7 :tlv_type, :value => 2
  bit9 :tlv_info_length, :value => lambda { subtype.num_bytes + port_id.length }
  uint8 :subtype
  string :port_id, :read_length => lambda { tlv_info_length - subtype.num_bytes }


  def get
    port_id
  end


  def set value
    self.port_id = value
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
