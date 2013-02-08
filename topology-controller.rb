$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "topology"
require "trema"
require "trema-extensions/packet-in"
require "view/text"


#
# This controller collects network topology information using LLDP.
#
# Usage:
#   $ trema run topology-controller.rb -c network.conf
#   $ trema run "topology-controller.rb graphviz" -c network.conf
#   $ trema run "topology-controller.rb graphviz /tmp/topology.png" -c network.conf
#
class TopologyController < Controller
  periodic_timer_event :flood_lldp_frames, 1


  def start
    view = View::Text.new

    if not ARGV[ 1 ].nil?
      if ARGV[ 1 ].downcase == "graphviz"
        require "view/graphviz"
        if ARGV.size > 2
          view = View::Graphviz.new( ARGV[ 2 ] )
        else
          view = View::Graphviz.new
        end
      end
    end

    @topology = Topology.new( view )
  end


  def switch_ready dpid
    send_message dpid, FeaturesRequest.new
  end


  def features_reply dpid, features_reply
    features_reply.ports.each do | each |
      next if each.down? or each.local?
      @topology.add_port dpid, each
    end
  end


  def switch_disconnected dpid
    @topology.delete_switch dpid
  end


  def port_status dpid, port_status
    updated_port = port_status.phy_port
    if updated_port.local?
      return
    elsif updated_port.down?
      @topology.delete_port dpid, updated_port
    elsif updated_port.up?
      @topology.add_port dpid, updated_port
    end
  end


  def packet_in dpid, packet_in
    return if not packet_in.lldp?
    @topology.add_link_by dpid, packet_in
  end


  ##############################################################################
  private
  ##############################################################################


  def flood_lldp_frames
    @topology.each_switch do | dpid, ports |
      send_lldp dpid, ports
    end
  end


  def send_lldp dpid, ports
    ports.each do | each |
      port_number = each.number
      send_packet_out(
        dpid,
        :actions => SendOutPort.new( port_number ),
        :data => Lldp.new( dpid, port_number ).to_binary
      )
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
