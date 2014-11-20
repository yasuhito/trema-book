# A simple openflow controller for benchmarking.
class CbenchSwitch < Trema::Controller
  def packet_in(datapath_id, message)
    send_flow_mod_add(
      datapath_id,
      cookie: 0,
      match: ExactMatch.from(message),
      buffer_id: message.buffer_id,
      actions: ActionOutput.new(message.in_port + 1)
    )
  end
end
