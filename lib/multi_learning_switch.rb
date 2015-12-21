require 'fdb'

# An OpenFlow controller that emulates multiple layer-2 switches.
class MultiLearningSwitch < Trema::Controller
  timer_event :age_fdbs, interval: 5.sec

  def start(_argv)
    @fdbs = {}
    logger.info "#{name} started."
  end

  def switch_ready(datapath_id)
    @fdbs[datapath_id] = FDB.new
  end

  def packet_in(datapath_id, packet_in)
    return if packet_in.destination_mac.reserved?
    @fdbs.fetch(datapath_id).learn(packet_in.source_mac, packet_in.in_port)
    flow_mod_and_packet_out packet_in
  end

  def age_fdbs
    @fdbs.each_value(&:age)
  end

  private

  def flow_mod_and_packet_out(packet_in)
    port_no = @fdbs.fetch(packet_in.dpid).lookup(packet_in.destination_mac)
    flow_mod(packet_in, port_no) if port_no
    packet_out(packet_in, port_no || :flood)
  end

  def flow_mod(packet_in, port_no)
    send_flow_mod_add(
      packet_in.datapath_id,
      match: ExactMatch.new(packet_in),
      actions: SendOutPort.new(port_no)
    )
  end

  def packet_out(packet_in, port_no)
    send_packet_out(
      packet_in.datapath_id,
      packet_in: packet_in,
      actions: SendOutPort.new(port_no)
    )
  end
end
