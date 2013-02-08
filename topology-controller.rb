$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "topology-db"
require "trema"
require "trema-extensions/packet-in"
require "trema-extensions/port"


class TopologyController < Controller
  periodic_timer_event :flood_lldp_frames, 1
  periodic_timer_event :show_topology, 3


  def start
    @topology_db = TopologyDB.new
  end


  def switch_ready dpid
    send_message dpid, FeaturesRequest.new
  end


  def features_reply dpid, message
    message.ports.each do | each |
      @topology_db.add_port dpid, each
    end
  end


  def switch_disconnected dpid
    @topology_db.delete_switch dpid
    info "Switch %#x deleted", dpid
  end


  def port_status dpid, message
    if message.phy_port.down?
      @topology_db.delete_port dpid, message.phy_port
      info "Port #{ message.phy_port.number } (Switch %#x) is DOWN", dpid
    elsif message.phy_port.up?
      @topology_db.add_port dpid, message.phy_port.dup
      info "Port #{ message.phy_port.number } (Switch %#x) is UP", dpid
    end
  end


  def packet_in dpid, message
    return if not message.lldp?
    @topology_db.add_link_by dpid, message
  end


  ##############################################################################
  private
  ##############################################################################


  def flood_lldp_frames
    @topology_db.each_pair do | dpid, ports |
      ports.each do | each |
        send_packet_out(
          dpid,
          :actions => SendOutPort.new( each.number ),
          :data => Lldp.new( dpid, each.number ).to_binary
        )
      end
    end
  end


  # FIXME
  def show_topology
    @topology_db.to_s.split( "\n" ).each do | each |
      info each
    end
    info "topology updated"
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
