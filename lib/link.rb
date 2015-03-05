require 'rubygems'
require 'pio/lldp'

#
# Edges between two switches.
#
class Link
  attr_reader :dpid_a
  attr_reader :dpid_b
  attr_reader :port_a
  attr_reader :port_b

  def initialize(dpid, packet_in)
    lldp = packet_in.data
    @dpid_a = lldp.dpid
    @dpid_b = dpid
    @port_a = lldp.port_number
    @port_b = packet_in.in_port
  end

  def ==(other)
    (@dpid_a == other.dpid_a) &&
      (@dpid_b == other.dpid_b) &&
      (@port_a == other.port_a) &&
      (@port_b == other.port_b)
  end

  def <=>(other)
    to_s <=> other.to_s
  end

  def to_s
    format '%#x-%#x', dpid_a, dpid_b
  end

  def connect_to?(port)
    dpid = port.dpid
    port_no = port.number
    ((@dpid_a == dpid) && (@port_a == port_no)) ||
      ((@dpid_b == dpid) && (@port_b == port_no))
  end
end
