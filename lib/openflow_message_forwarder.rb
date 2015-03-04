$LOAD_PATH.unshift __dir__
$LOAD_PATH.unshift File.join(__dir__, '../vendor/topology/lib')

require 'routing_switch'
require 'topology_controller'

# Forwards incoming OpenFlow messages to controllers
class OpenflowMessageForwarder < Trema::Controller
  timer_event :flood_lldp_frames, interval: 1.sec

  def start(argv)
    @topology_controller = TopologyController.new
    @topology_controller.start(argv)
    @routing_switch = RoutingSwitch.new
    @routing_switch.start(@topology_controller.topology)
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
    @routing_switch.packet_in(dpid, message) unless message.lldp?
  end

  def flood_lldp_frames
    @topology_controller.flood_lldp_frames
  end
end
