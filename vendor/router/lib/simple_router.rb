require 'arp_table'
require 'interface'
require 'routing_table'

# Simple implementation of L3 switch in OpenFlow1.0
# rubocop:disable ClassLength
class SimpleRouter < Trema::Controller
  def start(_args)
    load File.join(__dir__, '..', 'simple_router.conf')
    @interfaces = Interfaces.new(Config::INTERFACE)
    @arp_table = ArpTable.new
    @routing_table = RoutingTable.new(Config::ROUTE)
    logger.info "#{name} started."
  end

  def switch_ready(datapath_id)
    logger.info "#{datapath_id.to_hex} is connected"
    send_flow_mod_delete(datapath_id, match: Match.new)
  end

  # rubocop:disable MethodLength
  def packet_in(datapath_id, message)
    return unless to_me?(message)

    case message.data
    when Arp::Request
      handle_arp_request(datapath_id, message)
    when Arp::Reply
      handle_arp_reply(message)
    when Parser::IPv4Packet
      handle_ipv4(datapath_id, message)
    else
      fail 'Failed to handle packet_in data type'
    end
  end
  # rubocop:enable MethodLength

  private

  def to_me?(message)
    return true if message.destination_mac.broadcast?

    interface = @interfaces.find_by_port(message.in_port)
    return true if interface && interface.has?(message.destination_mac)
  end

  def handle_arp_request(datapath_id, message)
    port = message.in_port
    destination_mac = message.data.target_protocol_address
    interface = @interfaces.find_by_port_and_ip_address(port, destination_mac)
    return unless interface
    arp_reply = create_arp_reply_from(message, interface.mac_address)
    packet_out_raw(datapath_id, arp_reply, SendOutPort.new(interface.port))
  end

  def handle_arp_reply(message)
    @arp_table.update(message.in_port,
                      message.sender_protocol_address,
                      message.source_mac)
  end

  # rubocop:disable MethodLength
  def handle_ipv4(datapath_id, message)
    if should_forward?(message)
      forward(datapath_id, message)
    elsif message.data.ip_protocol == 1
      icmp = Icmp.read(message.raw_data)
      if icmp.icmp_type == 8
        handle_icmpv4_echo_request(datapath_id, message)
      else
        fail 'Failed to handle icmp type'
      end
    else
      fail 'Failed to handle ipv4 packet'
    end
  end
  # rubocop:enable MethodLength

  def should_forward?(message)
    !@interfaces.find_by_ip_address(message.data.ip_destination_address)
  end

  def handle_icmpv4_echo_request(datapath_id, message)
    interface = @interfaces.find_by_port(message.in_port)
    ip_source_address = message.data.ip_source_address
    arp_entry = @arp_table.lookup(ip_source_address)
    if arp_entry
      icmpv4_reply = create_icmpv4_reply(message)
      packet_out_raw(datapath_id, icmpv4_reply, SendOutPort.new(interface.port))
    else
      handle_unresolved_packet(datapath_id, interface, ip_source_address)
    end
  end

  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  def forward(datapath_id, message)
    next_hop = resolve_next_hop(message.data.ip_destination_address)

    interface = @interfaces.find_by_prefix(next_hop)
    return if !interface || interface.port == message.in_port

    arp_entry = @arp_table.lookup(next_hop)
    if arp_entry
      macsa = interface.mac_address
      macda = arp_entry.mac_address
      action = create_action_from(macsa, macda, interface.port)
      flow_mod(datapath_id, message, action)
      packet_out(datapath_id, message, action)
    else
      handle_unresolved_packet(datapath_id, interface, next_hop)
    end
  end
  # rubocop:enable AbcSize
  # rubocop:enable MethodLength

  def resolve_next_hop(ip_destination_address)
    interface = @interfaces.find_by_prefix(ip_destination_address)
    if interface
      ip_destination_address
    else
      @routing_table.lookup(ip_destination_address)
    end
  end

  def flow_mod(datapath_id, message, action)
    send_flow_mod_add(
      datapath_id,
      match: ExactMatch.new(message),
      actions: action
    )
  end

  def packet_out(datapath_id, packet, action)
    send_packet_out(
      datapath_id,
      packet_in: packet,
      actions: action
    )
  end

  def packet_out_raw(datapath_id, raw_data, action)
    send_packet_out(
      datapath_id,
      raw_data: raw_data,
      actions: action
    )
  end

  def handle_unresolved_packet(datapath_id, interface, ip_address)
    arp_request = create_arp_request_from(interface, ip_address)
    packet_out_raw(datapath_id, arp_request, SendOutPort.new(interface.port))
  end

  def create_action_from(source_mac, destination_mac, port)
    [
      SetEtherSourceAddress.new(source_mac),
      SetEtherDestinationAddress.new(destination_mac),
      SendOutPort.new(port)
    ]
  end

  def create_arp_request_from(interface, addr)
    Arp::Request.new(source_mac: interface.mac_address,
                     sender_protocol_address: interface.ip_address,
                     target_protocol_address: addr).to_binary
  end

  def create_arp_reply_from(message, replyaddr)
    Arp::Reply.new(
      destination_mac: message.data.source_mac,
      source_mac: replyaddr,
      sender_protocol_address: message.data.target_protocol_address,
      target_protocol_address: message.data.sender_protocol_address).to_binary
  end

  def create_icmpv4_reply(message)
    icmp_request = Icmp.read(message.raw_data)
    Icmp::Reply.new(identifier: icmp_request.icmp_identifier,
                    source_mac: message.destination_mac,
                    destination_mac: message.source_mac,
                    ip_destination_address: message.ip_source_address,
                    ip_source_address: message.ip_destination_address,
                    sequence_number: icmp_request.icmp_sequence_number,
                    echo_data: icmp_request.echo_data).to_binary
  end
end
# rubocop:enable ClassLength
