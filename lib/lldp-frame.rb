require "rubygems"
require "bindata"

require "chassis-id-tlv"
require "mac-address"
require "optional-tlv"
require "port-id-tlv"
require "ttl-tlv"


class LldpFrame < BinData::Record
  endian :big

  mac_address :destination_mac
  mac_address :source_mac
  uint16 :ethertype, :value => 0x88cc
  chassis_id_tlv :chassis_id
  port_id_tlv :port_id
  ttl_tlv :ttl, :initial_value => 120
  array :optional_tlv, :type => :optional_tlv, :read_until => lambda { element.end_of_lldpdu? }


  def dpid
    chassis_id
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
