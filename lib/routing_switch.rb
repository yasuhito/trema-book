# L2 routing switch
class RoutingSwitch < Trema::Controller
  def packet_in(datapath_id, message)
    send_flow_mod_add(
      datapath_id,
      match: ExactMatch.new(message),
      actions: SendOutPort.new(:flood)
    )
    send_packet_out(
      datapath_id,
      packet_in: message,
      actions: SendOutPort.new(:flood)
    )
  end
end
