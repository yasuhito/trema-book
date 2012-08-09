#
# Layer-2 switch implementation in Trema.
#
class LearningSwitch < Controller
  def start
    @fdb = {}
  end


  def packet_in datapath_id, message
    learn message.macsa, message.in_port
    port_no = port_no_from( message.macda )
    if port_no
      flow_mod message, port_no
      packet_out message, port_no
    else
      flood message
    end
  end


  private


  def learn mac, port_no
    @fdb[ mac ] = port_no
  end


  def port_no_from mac
    @fdb[ mac ]
  end


  def flow_mod message, port_no
    send_flow_mod_add(
      message.datapath_id,
      :match => ExactMatch.from( message ),
      :actions => SendOutPort.new( port_no )
    )
  end


  def packet_out message, port_no
    send_packet_out(
      message.datapath_id,
      :packet_in => message,
      :actions => SendOutPort.new( port_no )
    )
  end


  def flood message
    packet_out message, OFPP_FLOOD
  end
end
