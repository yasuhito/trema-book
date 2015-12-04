# Safety-net controller bridging legacy and OpenFlow networks.
class OneWayBridge < Trema::Controller
  def packet_in(datapath_id, message)
    out_port = { 1 => 2, 2 => 1 }.fetch(message.in_port)
    add_flow datapath_id, message.source_mac, message.in_port, out_port
    send_packet datapath_id, message, out_port
    add_drop_flow datapath_id, message.source_mac, out_port
  end

  def flow_removed(datapath_id, message)
    delete_flow datapath_id, message.match.source_mac_address
  end

  private

  def add_flow(datapath_id, source_mac, in_port, out_port)
    send_flow_mod_add(
      datapath_id,
      idle_timeout: 10 * 60,
      match: Match.new(in_port: in_port, source_mac_address: source_mac),
      actions: SendOutPort.new(out_port)
    )
  end

  def add_drop_flow(datapath_id, source_mac, in_port)
    send_flow_mod_add(
      datapath_id,
      idle_timeout: 10 * 60,
      match: Match.new(in_port: in_port, source_mac_address: source_mac)
    )
  end

  def send_packet(datapath_id, message, out_port)
    send_packet_out(
      datapath_id,
      packet_in: message,
      actions: SendOutPort.new(out_port)
    )
  end

  def delete_flow(datapath_id, source_mac)
    send_flow_mod_delete(
      datapath_id,
      match: Match.new(source_mac_address: source_mac)
    )
  end
end
