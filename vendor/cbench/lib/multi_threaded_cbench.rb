# A simple openflow controller for benchmarking (multi-threaded version).
class MultiThreadedCbench < Trema::Controller
  def start(_args)
    @work_queue = Queue.new
    10.times { start_worker_thread }
    logger.info "#{name} started."
  end

  def packet_in(datapath_id, packet_in)
    @work_queue.push [datapath_id, packet_in]
  end

  private

  def start_worker_thread
    Thread.new do
      loop do
        dpid, packet_in = @work_queue.pop
        send_flow_mod_add(dpid,
                          match: ExactMatch.new(packet_in),
                          buffer_id: packet_in.buffer_id,
                          actions: SendOutPort.new(packet_in.in_port + 1))
      end
    end
  end
end
