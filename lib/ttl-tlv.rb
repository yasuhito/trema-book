require "rubygems"
require "bindata"


class TtlTlv < BinData::Primitive
  endian :big

  bit7 :tlv_type, :value => 3
  bit9 :tlv_info_length, :value => 2
  string :ttl, :read_length => :tlv_info_length


  def get
    BinData::Int16be.read( ttl )
  end


  def set value
    self.ttl = BinData::Int16be.new( value ).to_binary_s
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
