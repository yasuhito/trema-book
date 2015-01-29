require 'English'

# Software patch-panel.
class PatchPanel < Trema::Controller
  def start(argv)
    @patch = parse(IO.read(argv[1] || './patch_panel.conf'))
  end

  def switch_ready(datapath_id)
    @patch.each do |port_a, port_b|
      make_patch datapath_id, port_a, port_b
    end
  end

  private

  def parse(config)
    config.split("\n").map do |each|
      fail unless /^(\d+)\s+(\d+)$/=~ each
      [$LAST_MATCH_INFO[1].to_i, $LAST_MATCH_INFO[2].to_i]
    end
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
