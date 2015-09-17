# Switch liveness monitor.
class SwitchMonitor < Trema::Controller
  timer_event :show_all_switches, interval: 10.sec

  def start(_args)
    @switches = []
    logger.info 'SwitchMonitor started.'
  end

  def switch_ready(dpid)
    @switches << dpid
    logger.info "#{dpid.to_hex} is up (All = #{all_switches_in_string})"
    send_message dpid, DescriptionStats::Request.new
  end

  def switch_disconnected(dpid)
    @switches -= [dpid]
    logger.info "#{dpid.to_hex} is down (All = #{all_switches_in_string})"
  end

  # rubocop:disable AbcSize
  def description_stats_reply(dpid, message)
    logger.info "Switch #{dpid.to_hex} manufacturer = #{message.manufacturer}"
    logger.info "Switch #{dpid.to_hex} hardware info = #{message.hardware}"
    logger.info "Switch #{dpid.to_hex} software info = #{message.software}"
    logger.info "Switch #{dpid.to_hex} serial number = #{message.serial_number}"
    logger.info "Switch #{dpid.to_hex} description = #{message.datapath}"
  end
  # rubocop:enable AbcSize

  private

  def show_all_switches
    logger.info "All = #{all_switches_in_string}"
  end

  def all_switches_in_string
    @switches.sort.map(&:to_hex).join(', ')
  end
end
