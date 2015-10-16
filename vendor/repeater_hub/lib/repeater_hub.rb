# Repeater hub controller.
class RepeaterHub < Trema::Controller
  def start(_args)
    logger.info "#{name} started."
  end

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
