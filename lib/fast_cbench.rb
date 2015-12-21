# A simple openflow controller for benchmarking (fast version).
class FastCbench < Trema::Controller
  def start(_args)
    logger.info "#{name} started."
  end

  def packet_in(dpid, packet_in)
    @flow_mod ||= create_flow_mod_binary(packet_in)
    send_message dpid, @flow_mod
  end

  private

  # rubocop:disable MethodLength
  def create_flow_mod_binary(packet_in)
    options = {
      command: :add,
      priority: 0,
      transaction_id: 0,
      idle_timeout: 0,
      hard_timeout: 0,
      buffer_id: packet_in.buffer_id,
      match: ExactMatch.new(packet_in),
      actions: SendOutPort.new(packet_in.in_port + 1)
    }
    FlowMod.new(options).to_binary.tap do |flow_mod|
      def flow_mod.to_binary
        self
      end
    end
  end
  # rubocop:enable MethodLength
end
