require 'English'

# Software patch-panel.
class PatchPanel < Trema::Controller
  def start(args)
    config_file = args[0] || 'patch_panel.conf'
    @patch = parse(IO.read(config_file))
    logger.info "PatchPanel started (config = #{config_file})."
  end

  def switch_ready(datapath_id)
    @patch.each do |port_a, port_b|
      make_patch datapath_id, port_a, port_b
    end
  end

  private

  def parse(config)
    config.each_line.map { |each| parse_line(each) }
  end

  def parse_line(line)
    fail "Invalid format: '#{line}'" unless /^(\d+)\s+(\d+)$/=~ line
    [$LAST_MATCH_INFO[1].to_i, $LAST_MATCH_INFO[2].to_i]
  end

  def make_patch(datapath_id, port_a, port_b)
    send_flow_mod_add(
      datapath_id,
      match: Match.new(in_port: port_a),
      actions: SendOutPort.new(port_b)
    )
    send_flow_mod_add(
      datapath_id,
      match: Match.new(in_port: port_b),
      actions: SendOutPort.new(port_a)
    )
  end
end
