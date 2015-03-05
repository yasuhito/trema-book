# L2 routing switch
class RoutingSwitch < Trema::Controller
  def start(topology)
    @topology = topology
    logger.info 'Routing Switch started.'
  end

  def packet_in(dpid, message)
    route = @topology.route(message.ip_source_address, message.ip_destination_address)
    return unless route
    route = route.slice!(1...-1)
    last = route.last
    while !route.empty?
      _in_port, dpid, out_port = route.slice!(0, 3)
      send_flow_mod_add(dpid,
                        match: ExactMatch.new(message),
                        actions: SendOutPort.new(out_port.split(':').last.to_i))
    end
    dpid, port = last.split(':').map(&:to_i)
    send_packet_out(dpid, packet_in: message, actions: SendOutPort.new(port))
  end
end
