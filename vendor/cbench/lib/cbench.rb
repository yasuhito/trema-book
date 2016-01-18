# A simple openflow controller for benchmarking.
class Cbench < Trema::Controller
  def start(_args)
    logger.info "#{name} started."
  end

  def packet_in(datapath_id, packet_in)
    send_flow_mod_add(
      datapath_id,
      match: ExactMatch.new(packet_in),
      buffer_id: packet_in.buffer_id,
      actions: SendOutPort.new(packet_in.in_port + 1)
    )
  end
end
