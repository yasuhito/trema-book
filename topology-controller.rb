$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "lldp-frame"
require "trema"


class TopologyController < Controller
  periodic_timer_event :send_features_request, 1
  periodic_timer_event :flood_lldp_frames, 1
  periodic_timer_event :show_topology, 3


  def start
    @switch_db = []
    @port_db = {}
    @topology = []
  end


  def switch_ready dpid
    @switch_db << dpid
  end


  def switch_disconnected dpid
    @switch_db -= [ dpid ]
    @port_db.delete dpid
    info "Switch %#x deleted", dpid
  end


  def features_reply dpid, message
    @port_db[ dpid ] = message.ports
  end


  def packet_in dpid, message
    return if not message.lldp?
    lldp = Lldp.read( message )
    @topology << format(
                   "%#x (port %d) <-> %#x (port %d)",
                   lldp.dpid, lldp.port_number,
                   dpid, message.in_port
                 )
  end


  ##############################################################################
  private
  ##############################################################################


  def send_features_request
    @switch_db.each do | each |
      send_message each, FeaturesRequest.new
    end
  end


  def flood_lldp_frames
    @port_db.each_pair do | dpid, ports |
      ports.select do | each |
        each.number != 65534 and each.up?
      end.each do | each |
        send_packet_out(
          dpid,
          :actions => SendOutPort.new( each.number ),
          :data => Lldp.new( dpid, each.number ).to_binary
        )
      end
    end
  end


  def show_topology
    return if @topology.empty?
    @topology.uniq.sort.each do | each |
      info each
    end
    info "topology updated"
    @topology = []
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
