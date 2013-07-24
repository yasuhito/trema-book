require "rubygems"
require "bindata"


class ChassisIdTlv < BinData::Primitive
  endian :big

  bit7 :tlv_type, :value => 1
  bit9 :tlv_info_length, :value => lambda { subtype.num_bytes + chassis_id.length }
  uint8 :subtype, :initial_value => 7
  string :chassis_id, :read_length => lambda { tlv_info_length - subtype.num_bytes }


  def get
    case subtype
    when 4
      chassis_id.unpack( "C6" ).collect { | each | "%02x" % each }.join( "" ).hex
    when 7
      BinData::Uint64be.read chassis_id
    else
      chassis_id
    end
  end


  def set value
    self.chassis_id = if value.kind_of?( Fixnum ) and subtype == 7
                        BinData::Uint64be.new( value ).to_binary_s
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
