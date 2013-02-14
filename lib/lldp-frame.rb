require "rubygems"

require "bindata"


class LldpFrame < BinData::Record
  class ChassisIdTlv < BinData::Primitive
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


  class PortIdTlv < BinData::Primitive
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


  class TtlTlv < BinData::Primitive
    bit7 :tlv_type, :value => 3
    bit9 :tlv_info_length, :value => 2
    string :ttl, :read_length => :tlv_info_length


    def get
      ttl
    end


    def set value
      self.ttl = BinData::Int16be.new( value ).to_binary_s
    end
  end


  class EndOfLldpdu < BinData::Primitive
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


  endian :big

  uint48 :destination_mac
  uint48 :source_mac
  uint16 :ethertype, :value => 0x88cc
  chassis_id_tlv :chassis_id
  port_id_tlv :port_id
  ttl_tlv :ttl
  end_of_lldpdu
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
