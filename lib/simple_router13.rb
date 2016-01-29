require 'interface'
require 'route'

# Simple implementation of L3 switch in OpenFlow1.0
# rubocop:disable ClassLength
class SimpleRouter13 < Trema::Controller
  INGRESS_TABLE = 0
  CLASSIFIER_TABLE = 1
  ARP_RESPONDER_TABLE = 2
  ROUTING_TABLE = 3
  INTERFACE_LOOKUP_TABLE = 4
  ARP_TABLE_LOOKUP_TABLE = 5
  EGRESS_TABLE = 6

  def start(_args)
    load File.join(__dir__, '..', 'simple_router.conf')
    Interface.load_configuration Configuration::INTERFACES
    Route.load_configuration Configuration::ROUTES
    @unresolved_packet_queue = Hash.new { [] }
    logger.info "#{name} started."
  end

  def switch_ready(dpid)
    clear_flow_entries(dpid)
    add_ingress_table_flow_entries(dpid)
    add_protocol_classifier_flow_entries(dpid)
    add_arp_responder_flow_entries(dpid)
    add_routing_table_flow_entries(dpid)
    add_interface_lookup_flow_entries(dpid)
    add_arp_table_lookup_flow_entries(dpid)
    add_arp_table_lookup_flow_entries(dpid)
    add_egress_flow_entry(dpid)
  end

  def packet_in(dpid, message)
    case message.data
    when Arp::Request, Arp::Reply
      packet_in_arp(dpid, message)
    when Parser::IPv4Packet
      packet_in_ipv4(dpid, message)
    else
      logger.debug "Dropping unsupported packet type: #{message.data.inspect}"
    end
  end

  def packet_in_arp(dpid, message)
    add_arp_entry(message.sender_protocol_address,
                  message.sender_hardware_address,
                  dpid)
    @unresolved_packet_queue[message.sender_protocol_address].each do |each|
      send_packet_out(dpid, raw_data: each, actions: SendOutPort.new(:table))
    end
    @unresolved_packet_queue[message.sender_protocol_address] = []
  end

  def packet_in_ipv4(dpid, message)
    dest_ip_address = IPv4Address.new(message.match.reg0.to_i)
    @unresolved_packet_queue[dest_ip_address] += [message.raw_data]
    send_packet_out(
      dpid,
      raw_data: Arp::Request.new(target_protocol_address: dest_ip_address,
                                 source_mac: '00:00:00:00:00:00',
                                 sender_protocol_address: '0.0.0.0').to_binary,
      actions: [NiciraRegLoad.new(message.match.reg1, :reg1),
                SendOutPort.new(:table)]
    )
  end

  private

  def clear_flow_entries(dpid)
    send_flow_mod_delete(dpid, match: Match.new)
  end

  def add_ingress_table_flow_entries(dpid)
    send_flow_mod_add(
      dpid,
      table_id: INGRESS_TABLE,
      match: Match.new,
      instructions: GotoTable.new(CLASSIFIER_TABLE)
    )
  end

  # rubocop:disable MethodLength
  def add_protocol_classifier_flow_entries(dpid)
    send_flow_mod_add(
      dpid,
      table_id: CLASSIFIER_TABLE,
      match: Match.new(ether_type: EthernetHeader::EtherType::ARP),
      instructions: GotoTable.new(ARP_RESPONDER_TABLE)
    )
    send_flow_mod_add(
      dpid,
      table_id: CLASSIFIER_TABLE,
      match: Match.new(ether_type: EthernetHeader::EtherType::IPV4),
      instructions: GotoTable.new(ROUTING_TABLE)
    )
  end
  # rubocop:enable MethodLength

  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  # rubocop:disable LineLength
  def add_arp_responder_flow_entries(dpid)
    Interface.each do |each|
      create_and_send_arp_reply_actions = [
        SendOutPort.new(:controller),
        NiciraRegMove.new(from: :source_mac_address,
                          to: :destination_mac_address),
        SetSourceMacAddress.new(each.mac_address),
        SetArpOperation.new(Arp::Reply::OPERATION),
        NiciraRegMove.new(from: :arp_sender_hardware_address,
                          to: :arp_target_hardware_address),
        NiciraRegMove.new(from: :arp_sender_protocol_address,
                          to: :arp_target_protocol_address),
        SetArpSenderHardwareAddress.new(each.mac_address),
        SetArpSenderProtocolAddress.new(each.ip_address),
        NiciraRegLoad.new(0xffff, :in_port),
        NiciraRegLoad.new(each.port_number, :reg1)
      ]
      send_flow_mod_add(
        dpid,
        table_id: ARP_RESPONDER_TABLE,
        match: Match.new(ether_type: EthernetHeader::EtherType::ARP,
                         arp_operation: Arp::Request::OPERATION,
                         in_port: each.port_number,
                         arp_target_protocol_address: each.ip_address),
        instructions: [Apply.new(create_and_send_arp_reply_actions),
                       GotoTable.new(EGRESS_TABLE)]
      )

      send_flow_mod_add(
        dpid,
        table_id: ARP_RESPONDER_TABLE,
        match: Match.new(ether_type: EthernetHeader::EtherType::ARP,
                         arp_operation: Arp::Reply::OPERATION,
                         in_port: each.port_number,
                         arp_target_protocol_address: each.ip_address),
        instructions: Apply.new(SendOutPort.new(:controller))
      )

      send_flow_mod_add(
        dpid,
        table_id: ARP_RESPONDER_TABLE,
        match: Match.new(ether_type: EthernetHeader::EtherType::ARP,
                         reg1: each.port_number),
        instructions: [Apply.new([SetSourceMacAddress.new(each.mac_address),
                                  SetArpSenderHardwareAddress.new(each.mac_address),
                                  SetArpSenderProtocolAddress.new(each.ip_address)]),
                       GotoTable.new(EGRESS_TABLE)]
      )
    end
  end
  # rubocop:enable MethodLength
  # rubocop:enable AbcSize
  # rubocop:enable LineLength

  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  # rubocop:disable LineLength
  def add_routing_table_flow_entries(dpid)
    Route.each do |each|
      # Static route
      send_flow_mod_add(
        dpid,
        table_id: ROUTING_TABLE,
        priority: each.netmask_length * 100,
        match: Match.new(ether_type: EthernetHeader::EtherType::IPV4,
                         ipv4_destination_address: each.destination,
                         ipv4_destination_address_mask: each.netmask),
        instructions: [Apply.new(NiciraRegLoad.new(each.next_hop.to_i, :reg0)),
                       GotoTable.new(INTERFACE_LOOKUP_TABLE)]
      )
    end

    Interface.each do |each|
      # Search ConnectedRoute
      send_flow_mod_add(
        dpid,
        table_id: ROUTING_TABLE,
        priority: each.netmask_length + 40_000,
        match: Match.new(ether_type: EthernetHeader::EtherType::IPV4,
                         ipv4_destination_address: each.network_address,
                         ipv4_destination_address_mask: each.netmask),
        instructions: [Apply.new(NiciraRegMove.new(from: :ipv4_destination_address, to: :reg0)),
                       GotoTable.new(INTERFACE_LOOKUP_TABLE)]
      )
    end
  end
  # rubocop:enable MethodLength
  # rubocop:enable AbcSize
  # rubocop:enable LineLength

  # rubocop:disable MethodLength
  def add_interface_lookup_flow_entries(dpid)
    Interface.each do |each|
      # Search for output interface port
      send_flow_mod_add(
        dpid,
        table_id: INTERFACE_LOOKUP_TABLE,
        match: Match.new(reg0: each.network_address.to_i,
                         reg0_mask: each.netmask.to_i),
        instructions: [Apply.new([NiciraRegLoad.new(each.port_number, :reg1),
                                  SetSourceMacAddress.new(each.mac_address)]),
                       GotoTable.new(ARP_TABLE_LOOKUP_TABLE)]
      )
    end
  end
  # rubocop:enable MethodLength

  def add_arp_table_lookup_flow_entries(dpid)
    send_flow_mod_add(
      dpid,
      table_id: ARP_TABLE_LOOKUP_TABLE,
      priority: 1,
      match: Match.new(ether_type: EthernetHeader::EtherType::IPV4),
      instructions: Apply.new(SendOutPort.new(:controller))
    )
  end

  def add_egress_flow_entry(dpid)
    send_flow_mod_add(
      dpid,
      table_id: EGRESS_TABLE,
      match: Match.new,
      instructions: Apply.new(NiciraSendOutPort.new(:reg1))
    )
  end

  def add_arp_entry(ip_address, mac_address, dpid)
    send_flow_mod_add(
      dpid,
      table_id: ARP_TABLE_LOOKUP_TABLE,
      priority: 2,
      match: Match.new(ether_type: EthernetHeader::EtherType::IPV4,
                       reg0: IPv4Address.new(ip_address).to_i),
      instructions: [Apply.new(SetDestinationMacAddress.new(mac_address)),
                     GotoTable.new(EGRESS_TABLE)]
    )
  end
end
# rubocop:enable ClassLength
