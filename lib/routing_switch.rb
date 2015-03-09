$LOAD_PATH.unshift __dir__
$LOAD_PATH.unshift File.join(__dir__, '../vendor/topology/lib')

require 'drb'
require 'optparse'
require 'path_manager'
require 'sliceable_switch'
require 'topology_controller'

# L2 routing switch
class RoutingSwitch < Trema::Controller
  # Command-line options of RoutingSwitch
  class Options
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
    options = Options.new
    options.parse(args)
    @path_manager = options.slicing ? SliceableSwitch.new : PathManager.new
    @topology_controller = TopologyController.new
    @topology_controller.start([])
    @topology_controller.add_observer(@path_manager)
    @path_manager.start
    logger.info 'Routing Switch started.'
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
    @path_manager.packet_in(dpid, message) unless message.lldp?
  end

  def add_slice(name)
    @path_manager.add_slice(name)
  end

  def add_mac_to_slice(mac_address, slice)
    @path_manager.add_mac_to_slice(mac_address, slice)
  end

  private

  def flood_lldp_frames
    @topology_controller.flood_lldp_frames
  end
end
