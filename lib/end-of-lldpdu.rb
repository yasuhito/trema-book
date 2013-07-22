require "rubygems"
require "bindata"


class EndOfLldpdu < BinData::Primitive
  endian :big

  bit7 :tlv_type, :value => 0
  bit9 :tlv_info_length, :value => 0
  string :tlv_info_string, :value => ""


  def get
    self
  end


  def set value
    # Read Only
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
