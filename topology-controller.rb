$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "topology"
require "topology-view"
require "trema"
require "trema-extensions/packet-in"


class TopologyController < Controller
  periodic_timer_event :flood_lldp_frames, 1


  def start
    @topology = Topology.new( TopologyView.new )
  end


  def switch_ready dpid
    send_message dpid, FeaturesRequest.new
  end


  def features_reply dpid, message
    message.ports.each do | each |
      @topology.add_port dpid, each
    end
  end


  def switch_disconnected dpid
    @topology.delete_switch dpid
    info "Switch %#x deleted", dpid
  end


  def port_status dpid, message
    if message.phy_port.down?
      @topology.delete_port dpid, message.phy_port
      info "Port #{ message.phy_port.number } (Switch %#x) is DOWN", dpid
    elsif message.phy_port.up?
      @topology.add_port dpid, message.phy_port.dup
      info "Port #{ message.phy_port.number } (Switch %#x) is UP", dpid
    end
  end


  def packet_in dpid, message
    return if not message.lldp?
    @topology.add_link_by dpid, message
  end


  ##############################################################################
  private
  ##############################################################################


  def flood_lldp_frames
    @topology.each_pair do | dpid, ports |
      ports.each do | each |
        send_packet_out(
          dpid,
          :actions => SendOutPort.new( each.number ),
          :data => Lldp.new( dpid, each.number ).to_binary
        )
      end
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
