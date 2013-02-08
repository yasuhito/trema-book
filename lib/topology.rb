require "link"
require "lldp-frame"
require "observer"
require "trema"
require "trema-extensions/port"


class Topology
  include Observable
  extend Forwardable


  def_delegator :@ports, :each_pair, :each_ports


  def initialize controller
    @ports = Hash.new { [].freeze }
    @links = []
    add_observer controller
  end


  def delete_switch dpid
    @ports[ dpid ].each do | each |
      delete_port_by_number dpid, each
    end
    @ports.delete dpid
  end


  def add_port dpid, port
    return if port.local? or port.down?
    @ports[ dpid ] += [ port.number ]
  end


  def delete_port dpid, port
    delete_port_by_number dpid, port.number
  end


  def add_link_by dpid, packet_in
    raise "Not an LLDP packet!" if not packet_in.lldp?

    lldp = Lldp.read( packet_in )
    link = Link.new( lldp.dpid, lldp.port_number, dpid, packet_in.in_port )

    if not @links.include?( link )
      @links << link
      changed
      notify_observers @links.sort
    end
  end


  ##############################################################################
  private
  ##############################################################################


  def delete_port_by_number dpid, number
    @ports[ dpid ] -= [ number ]
    @links.collect do | each |
      if ( ( each.dpid1 == dpid ) and ( each.port1 == number ) ) or
          ( ( each.dpid2 == dpid ) and ( each.port2 == number ) )
        changed
        @links -= [ each ]
      end
    end
    notify_observers @links.sort
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
