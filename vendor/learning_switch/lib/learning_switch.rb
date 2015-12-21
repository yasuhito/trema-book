require 'fdb'

# An OpenFlow controller that emulates a layer-2 switch.
class LearningSwitch < Trema::Controller
  timer_event :age_fdb, interval: 5.sec

  def start(_argv)
    @fdb = FDB.new
    logger.info "#{name} started."
  end

  def packet_in(_datapath_id, packet_in)
    return if packet_in.destination_mac.reserved?
    @fdb.learn packet_in.source_mac, packet_in.in_port
    flow_mod_and_packet_out packet_in
  end

  def age_fdb
    @fdb.age
  end

  private

  def flow_mod_and_packet_out(packet_in)
    port_no = @fdb.lookup(packet_in.destination_mac)
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
