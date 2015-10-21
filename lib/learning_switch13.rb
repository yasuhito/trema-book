# An OpenFlow controller that emulates a layer-2 switch.
class LearningSwitch13 < Trema::Controller
  INGRESS_FILTERING_TABLE_ID = 0
  FORWARDING_TABLE_ID = 1

  AGING_TIME = 180

  def start(_args)
    logger.info 'LearningSwitch started.'
  end

  def switch_ready(datapath_id)
    add_multicast_mac_drop_flow_entry datapath_id
    add_default_forwarding_flow_entry datapath_id
    add_default_flooding_flow_entry datapath_id
  end

  def packet_in(datapath_id, message)
    add_forwarding_flow_entry(datapath_id, message)
  end

  private

  def add_forwarding_flow_entry(datapath_id, message)
    send_flow_mod_add(
      datapath_id,
      table_id: FORWARDING_TABLE_ID,
      idle_timeout: AGING_TIME,
      priority: 2,
      match: Match.new(ether_destination_address: message.source_mac),
      instructions: Apply.new(SendOutPort.new(message.in_port))
    )
  end

  def add_default_flooding_flow_entry(datapath_id)
    send_flow_mod_add(
      datapath_id,
      table_id: FORWARDING_TABLE_ID,
      idle_timeout: 0,
      priority: 1,
      match: Match.new,
      instructions: Apply.new([SendOutPort.new(:controller),
                               SendOutPort.new(:flood)])
    )
  end

  def add_multicast_mac_drop_flow_entry(datapath_id)
    send_flow_mod_add(
      datapath_id,
      table_id: INGRESS_FILTERING_TABLE_ID,
      idle_timeout: 0,
      priority: 2,
      match: Match.new(ether_destination_address: '01:00:5e:00:00:00',
                       ether_destination_address_mask: 'ff:ff:ff:00:00:00')
    )
  end

  def add_default_forwarding_flow_entry(datapath_id)
    send_flow_mod_add(
      datapath_id,
      table_id: INGRESS_FILTERING_TABLE_ID,
      idle_timeout: 0,
      priority: 2,
      match: Match.new,
      instructions: GotoTable.new(FORWARDING_TABLE_ID)
    )
  end
end
