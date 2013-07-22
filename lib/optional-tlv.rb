require "port-description-value"
require "system-name-value"
require "system-description-value"
require "system-capabilities-value"
require "management-address-value"
require "end-of-lldpdu-value"


class OptionalTlv < BinData::Record
  endian :big

  bit7 :tlv_type
  bit9 :tlv_info_length
  choice :tlv_value, :selection => :chooser do
    end_of_lldpdu_value 0, :read_length => 0
    port_description_value 4, :read_length => :tlv_info_length
    system_name_value 5, :read_length => :tlv_info_length
    system_description_value 6, :read_length => :tlv_info_length
    system_capabilities_value 7, :read_length => :tlv_info_length
    management_address_value 8, :read_length => :tlv_info_length
    stringz "unknown", :read_length => :tlv_info_length
  end


  def chooser
    if tlv_type == 0 or ( 4 <= tlv_type and tlv_type <= 8 )
      tlv_type
    else
      "unknown"
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
