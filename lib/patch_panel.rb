# Software patch-panel.
class PatchPanel < Trema::Controller
  def start
    @patch = []
    File.open('./patch_panel.conf').each_line do |each|
      if /^(\d+)\s+(\d+)$/ =~ each
        @patch << [Regexp.last_match[1].to_i, Regexp.last_match[2].to_i]
      end
    end
  end

  def switch_ready(datapath_id)
    @patch.each do |port_a, port_b|
      make_patch datapath_id, port_a, port_b
    end
  end

  private

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
