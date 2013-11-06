# -*- coding: utf-8 -*-
require 'rubygems'
require 'pio/lldp'

#
# Edges between two switches.
#
class Link
  attr_reader :dpid1
  attr_reader :dpid2
  attr_reader :port1
  attr_reader :port2

  def initialize(dpid, packet_in)
    lldp = Pio::Lldp.read(packet_in.data)
    @dpid1 = lldp.dpid
    @dpid2 = dpid
    @port1 = lldp.port_number
    @port2 = packet_in.in_port
  end

  def ==(other)
    (@dpid1 == other.dpid1) &&
      (@dpid2 == other.dpid2) &&
      (@port1 == other.port1) &&
      (@port2 == other.port2)
  end

  def <=>(other)
    to_s <=> other.to_s
  end

  def to_s
    format '%#x (port %d) <-> %#x (port %d)', dpid1, port1, dpid2, port2
  end

  def has?(dpid, port)
    ((@dpid1 == dpid) && (@port1 == port)) ||
      ((@dpid2 == dpid) && (@port2 == port))
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
