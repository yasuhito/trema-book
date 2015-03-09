$LOAD_PATH.unshift __dir__
$LOAD_PATH.unshift File.join(__dir__, '../vendor/topology/lib')

require 'drb'
require 'optparse'
require 'routing_switch'
require 'sliceable_switch'
require 'topology_controller'

# Forwards incoming OpenFlow messages to controllers
class OpenflowMessageForwarder < Trema::Controller
  # Command-line options of OpenflowMessageForwarder
  class CommandLine
    attr_reader :slicing

    def initialize
      @opts = OptionParser.new
      @opts.on('-s', '--slicing') { @slicing = true }
    end

    def parse(args)
      @opts.parse [__FILE__] + args
    end
  end

  timer_event :flood_lldp_frames, interval: 1.sec

  # This method smells of :reek:TooManyStatements but ignores them
  def start(args)
    command_line = CommandLine.new
    command_line.parse(args)
    @switch_controller =
      command_line.slicing ? SliceableSwitch.new : RoutingSwitch.new
    @topology_controller = TopologyController.new
    @topology_controller.start([])
    @topology_controller.add_observer(@switch_controller)
    @switch_controller.start
  end

  def switch_ready(dpid)
    @topology_controller.switch_ready dpid
  end

  def features_reply(dpid, features_reply)
    @topology_controller.features_reply dpid, features_reply
  end

  def switch_disconnected(dpid)
    @topology_controller.switch_disconnected(dpid)
  end

  def port_modify(dpid, port_status)
    @topology_controller.port_modify(dpid, port_status)
  end

  def packet_in(dpid, message)
    @topology_controller.packet_in(dpid, message)
    @switch_controller.packet_in(dpid, message) unless message.lldp?
  end

  def add_slice(name)
    @switch_controller.add_slice(name)
  end

  def add_mac_to_slice(mac_address, slice)
    @switch_controller.add_mac_to_slice(mac_address, slice)
  end

  private

  def flood_lldp_frames
    @topology_controller.flood_lldp_frames
  end
end
