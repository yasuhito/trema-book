require "forwardable"
require "link"
require "observer"
require "trema-extensions/port"


#
# Topology information containing the list of known switches, ports,
# and links.
#
class Topology
  include Observable
  extend Forwardable


  def_delegator :@ports, :each_pair, :each_switch
  def_delegator :@links, :each, :each_link


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


  def update_port dpid, port
    if port.down?
      delete_port dpid, port
    elsif port.up?
      add_port dpid, port
    end
  end


  def add_port dpid, port
    @ports[ dpid ] += [ port ]
  end


  def delete_port dpid, port
    @ports[ dpid ] -= [ port ]
    delete_link_by dpid, port
  end


  def add_link_by dpid, packet_in
    raise "Not an LLDP packet!" if not packet_in.lldp?

    link = Link.new( dpid, packet_in )
    if not @links.include?( link )
      @links << link
      @links.sort!
      changed
      notify_observers self
    end
  end


  ##############################################################################
  private
  ##############################################################################


  def delete_link_by dpid, port
    @links.each do | each |
      if each.has?( dpid, port.number )
        changed
        @links -= [ each ]
      end
    end
    notify_observers self
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
