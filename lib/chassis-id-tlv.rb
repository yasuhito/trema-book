require "rubygems"
require "bindata"


class ChassisIdTlv < BinData::Primitive
  endian :big

  bit7 :tlv_type, :value => 1
  bit9 :tlv_info_length, :value => lambda { subtype.num_bytes + chassis_id.length }
  uint8 :subtype
  string :chassis_id, :read_length => lambda { tlv_info_length - subtype.num_bytes }


  def get
    chassis_id
  end


  def set value
    self.chassis_id = value
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
