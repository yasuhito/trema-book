require "rubygems"

require "bindata"
require "forwardable"
require "lldp-frame"


class Lldp
  extend Forwardable


  def self.read raw_data
    lldp_frame = LldpFrame.read( raw_data )
    new lldp_frame.dpid, lldp_frame.port_id
  end


  def initialize dpid, port_number, destination_mac = "01:80:c2:00:00:0e"
    @frame = LldpFrame.new
    @frame.destination_mac = destination_mac
    @frame.source_mac = "11:22:33:44:55:66"  # FIXME
    @frame.chassis_id = dpid
    @frame.port_id = port_number
  end


  def_delegator :@frame, :dpid
  def_delegator :@frame, :optional_tlv
  def_delegator :@frame, :port_id, :port_number


  def to_binary
    @frame.to_binary_s + "\000" * ( 64 - @frame.num_bytes )
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
