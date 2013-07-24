require "rubygems"
require "bindata"


class PortIdTlv < BinData::Primitive
  endian :big

  bit7 :tlv_type, :value => 2
  bit9 :tlv_info_length, :initial_value => lambda { port_id.num_bytes + 1 }
  uint8 :subtype, :initial_value => 7
  string :port_id, :read_length => lambda { tlv_info_length - 1 }


  def get
    if subtype == 7
      BinData::Uint32be.read port_id
    else
      port_id
    end
  end


  def set value
    self.port_id = if value.kind_of?( Fixnum ) and subtype == 7
                     BinData::Uint32be.new( value ).to_binary_s
                   else
                     value
                   end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
