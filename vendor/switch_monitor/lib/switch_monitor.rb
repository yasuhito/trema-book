# Switch liveness monitor.
class SwitchMonitor < Trema::Controller
  def start(_args)
    @switches = []
    logger.info 'SwitchMonitor started.'
  end

  def switch_ready(datapath_id)
    @switches << datapath_id
    logger.info "#{datapath_id.to_hex} is up (all = #{switches_in_string})"
  end

  def switch_disconnected(datapath_id)
    @switches -= [datapath_id]
    logger.info "#{datapath_id.to_hex} is down (all = #{switches_in_string})"
  end

  private

  def switches_in_string
    @switches.sort.map(&:to_hex).join(', ')
  end
end
