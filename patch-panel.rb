class PatchPanel < Controller
  def start
    @patch = []
    File.open( "./patch-panel.conf" ).each_line do | each |
      if /^(\d+)\s+(\d+)$/=~ each
        @patch << [ $1.to_i, $2.to_i ]
      end
    end
  end


  def switch_ready datapath_id
    @patch.each do | port_a, port_b |
      make_patch datapath_id, port_a, port_b
    end
  end


  ##############################################################################
  private
  ##############################################################################


  def make_patch datapath_id, port_a, port_b
    send_flow_mod_add(
      datapath_id,
      :match => Match.new( :in_port => port_a ),
      :actions => ActionOutput.new( port_b )
    )
    send_flow_mod_add(
      datapath_id,
      :match => Match.new( :in_port => port_b ),
      :actions => ActionOutput.new( port_a )
    )
  end
end
