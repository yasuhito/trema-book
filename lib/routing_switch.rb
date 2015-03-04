# L2 routing switch
class RoutingSwitch < Trema::Controller
  attr_writer :topology

  def start(topology)
    @topology = topology
    logger.info 'Routing Switch started.'
  end

  def packet_in(dpid, message)
    dpid, port = @topology.find_dpid_and_port(message.destination_mac)
    return unless dpid
    send_packet_out(dpid, packet_in: message, actions: SendOutPort.new(port))
  end
end
