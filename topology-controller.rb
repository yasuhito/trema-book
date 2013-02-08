$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "topology"
require "topology-view"
require "trema"
require "trema-extensions/packet-in"


#
# This controller collects network topology information using LLDP.
#
class TopologyController < Controller
  periodic_timer_event :flood_lldp_frames, 1


  def start
    @topology = Topology.new( TopologyView.new )
  end


  def switch_ready dpid
    send_message dpid, FeaturesRequest.new
    info "Switch %#x is UP", dpid
  end


  def features_reply dpid, features_reply
    features_reply.ports.each do | each |
      next if each.down?
      @topology.add_port dpid, each
    end
  end


  def switch_disconnected dpid
    @topology.delete_switch dpid
    info "Switch %#x is DOWN", dpid
  end


  def port_status dpid, port_status
    updated_port = port_status.phy_port
    if updated_port.down?
      delete_port dpid, updated_port
    elsif updated_port.up?
      add_port dpid, updated_port
    end
  end


  def packet_in dpid, packet_in
    return if not packet_in.lldp?
    @topology.add_link_by dpid, packet_in
  end


  ##############################################################################
  private
  ##############################################################################


  def delete_port dpid, port
    @topology.delete_port dpid, port
    info "Port #{ port.number } (Switch %#x) is DOWN", dpid
  end


  def add_port dpid, port
    @topology.add_port dpid, port.dup
    info "Port #{ port.number } (Switch %#x) is UP", dpid
  end


  def flood_lldp_frames
    @topology.each_ports do | dpid, ports |
      send_lldp dpid, ports
    end
  end


  def send_lldp dpid, ports
    ports.each do | each |
      send_packet_out(
        dpid,
        :actions => SendOutPort.new( each ),
        :data => Lldp.new( dpid, each ).to_binary
      )
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
