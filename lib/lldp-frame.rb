require "rubygems"
require "bindata"

require "chassis-id-tlv"
require "end-of-lldpdu"
require "optional-tlv"
require "port-id-tlv"
require "ttl-tlv"


class LldpFrame < BinData::Record
  endian :big

  uint48 :destination_mac
  uint48 :source_mac
  uint16 :ethertype, :value => 0x88cc
  chassis_id_tlv :chassis_id
  port_id_tlv :port_id
  ttl_tlv :ttl
  array :optional_tlv, :type => :optional_tlv, :read_until => :eof


  def dpid
    BinData::Uint64be.read( "\000\000" + chassis_id ).to_i
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
