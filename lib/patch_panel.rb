# Software patch-panel.
class PatchPanel < Trema::Controller
  def start(_args)
    @patch = {}
    logger.info 'PatchPanel started.'
  end

  def switch_ready(dpid)
    if @patch.key?(dpid)
      @patch[dpid].each do |port_a, port_b|
        delete_flow_entries dpid, port_a, port_b
        add_flow_entries dpid, port_a, port_b
      end
    else
      @patch[dpid] = []
    end
  end

  def create_patch(dpid, port_a, port_b)
    add_flow_entries dpid, port_a, port_b
    @patch[dpid] << [port_a, port_b].sort
  end

  def delete_patch(dpid, port_a, port_b)
    delete_flow_entries dpid, port_a, port_b
    @patch[dpid].delete [port_a, port_b].sort
  end

  private

  def add_flow_entries(dpid, port_a, port_b)
    check_dpid dpid
    send_flow_mod_add(dpid,
                      match: Match.new(in_port: port_a),
                      actions: SendOutPort.new(port_b))
    send_flow_mod_add(dpid,
                      match: Match.new(in_port: port_b),
                      actions: SendOutPort.new(port_a))
  end

  def delete_flow_entries(dpid, port_a, port_b)
    check_dpid dpid
    send_flow_mod_delete(dpid, match: Match.new(in_port: port_a))
    send_flow_mod_delete(dpid, match: Match.new(in_port: port_b))
  end

  def check_dpid(dpid)
    fail "Unknown dpid: #{dpid.to_hex}" unless @patch.key?(dpid)
  end
end
