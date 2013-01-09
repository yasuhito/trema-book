$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "lldp-frame"
require "trema"


class TopologyController < Controller
  periodic_timer_event :flood_lldp_frames, 5


  def start
    @switch_db = {}
  end


  def switch_ready dpid
    send_message dpid, FeaturesRequest.new
  end


  def features_reply dpid, message
    @switch_db[ dpid ] = message.ports[ 0 ].hw_addr
  end


  def packet_in dpid, message
    return if not message.lldp?
    info format( "%#x <-> %#x", @switch_db.invert[ Lldp.read( message ).source_mac ], dpid )
  end


  ##############################################################################
  private
  ##############################################################################


  def flood_lldp_frames
    @switch_db.each_pair do | dpid, mac |
      send_packet_out(
        dpid,
        :actions => SendOutPort.new( OFPP_FLOOD ),
        :data => Lldp.new( mac ).to_binary
      )
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
