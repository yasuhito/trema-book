$LOAD_PATH.unshift __dir__
$LOAD_PATH.unshift File.join(__dir__, '../vendor/topology/lib')

require 'topology_controller'

# L2 routing switch
class RoutingSwitch < Trema::Controller
  timer_event :flood_lldp_frames, interval: 1.sec

  def start(argv)
    @topology_controller = TopologyController.new
    @topology_controller.start(argv)
    logger.info 'Routing switch started.'
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
    send_flow_mod_add(dpid,
                      match: ExactMatch.new(message),
                      actions: SendOutPort.new(:flood))
    send_packet_out(dpid,
                    packet_in: message,
                    actions: SendOutPort.new(:flood))
  end

  def flood_lldp_frames
    @topology_controller.flood_lldp_frames
  end
end
