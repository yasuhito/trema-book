require "link"
require "lldp-frame"
require "observer"
require "trema"
require "trema-extensions/port"


class Topology
  include Observable
  extend Forwardable


  def_delegators :@ports, :each_pair


  def initialize controller
    @ports = Hash.new { [].freeze }
    @links = []
    add_observer controller
  end


  def delete_switch dpid
    @ports[ dpid ].each do | each |
      delete_port dpid, each
    end
    @ports.delete dpid
  end


  def add_port dpid, port
    return if port.local? or port.down?
    @ports[ dpid ] += [ port ]
  end


  def delete_port dpid, port
    @ports[ dpid ] -= [ port ]
    @links.collect do | each |
      if ( ( each.dpid1 == dpid ) and ( each.port1 == port.number ) ) or
          ( ( each.dpid2 == dpid ) and ( each.port2 == port.number ) )
        changed
        @links -= [ each ]
      end
    end
    notify_observers @links.sort
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
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
