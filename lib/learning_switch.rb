$LOAD_PATH.unshift File.expand_path(__dir__)

require 'fdb'

# An OpenFlow controller that emulates a layer-2 switch.
class LearningSwitch < Trema::Controller
  timer_event :age_fdb, interval: 5.sec

  def start(_argv)
    @fdb = FDB.new
  end

  def packet_in(datapath_id, message)
    return if message.destination_mac.reserved?

    @fdb.learn message.source_mac, message.in_port
    port_no = @fdb.lookup(message.destination_mac)
    if port_no
      flow_mod datapath_id, message, port_no
      packet_out datapath_id, message, port_no
    else
      flood datapath_id, message
    end
  end

  def age_fdb
    @fdb.age
  end

  private

  def flow_mod(datapath_id, message, port_no)
    send_flow_mod_add(
      datapath_id,
      match: ExactMatch.new(message),
      actions: SendOutPort.new(port_no)
    )
  end

  def packet_out(datapath_id, message, port_no)
    send_packet_out(
      datapath_id,
      packet_in: message,
      actions: SendOutPort.new(port_no)
    )
  end

  def flood(datapath_id, message)
    packet_out datapath_id, message, :flood
  end
end
