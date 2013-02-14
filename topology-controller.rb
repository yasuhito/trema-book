$LOAD_PATH.unshift File.expand_path( File.join File.dirname( __FILE__ ), "lib" )

require "rubygems"

require "gli"
require "topology"
require "trema"
require "trema-extensions/packet-in"
require "view/text"


#
# This controller collects network topology information using LLDP.
#
class TopologyController < Controller
  periodic_timer_event :flood_lldp_frames, 1


  def start
    @view = View::Text.new
    parse ARGV.dup
    @topology = Topology.new( @view )
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


  def parse argv
    GLI::App.program_desc "Topology discovery controller"

    GLI::App.flag [ :d, :destination_mac ]

    GLI::App.pre do | global_options, command, options, args |
      if global_options[ :destination_mac ]
        @destination_mac = Mac.new( global_options[ :destination_mac ] )
      end
    end

    GLI::App.default_command :text

    GLI::App.desc "Displays topology information (text mode)"
    GLI::App.command :text do | c |
      c.action do | global_options, options, args |
        @view = View::Text.new
      end
    end

    GLI::App.desc "Displays topology information (Graphviz mode)"
    arg_name "output_file"
    GLI::App.command :graphviz do | c |
      c.action do | global_options, options, args |
        require "view/graphviz"
        if args.empty?
          @view = View::Graphviz.new
        else
          @view = View::Graphviz.new( args[ 0 ] )
        end
      end
    end

    GLI::App.run argv
  end


  def flood_lldp_frames
    @topology.each_switch do | dpid, ports |
      send_lldp dpid, ports
    end
  end


  def send_lldp dpid, ports
    ports.each do | each |
      port_number = each.number
      lldp_binary = if @destination_mac
                      Lldp.new( dpid, port_number, @destination_mac.value ).to_binary
                    else
                      Lldp.new( dpid, port_number ).to_binary
                    end
      send_packet_out(
        dpid,
        :actions => SendOutPort.new( port_number ),
        :data => lldp_binary
      )
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
