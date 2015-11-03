require 'arp_table'
require 'interfaces'
require 'routing_table'

# Simple implementation of L3 switch in OpenFlow1.0
# rubocop:disable ClassLength
class SimpleRouter < Trema::Controller
  def start(_args)
    load File.join(__dir__, '..', 'simple_router.conf')
    @unresolved = Hash.new { [] }
    @interfaces = Interfaces.new(Configuration::INTERFACES)
    @arp_table = ArpTable.new
    @routing_table = RoutingTable.new(Configuration::ROUTES)
    logger.info "#{name} started."
  end

  def switch_ready(datapath_id)
    send_flow_mod_delete(datapath_id, match: Match.new)
  end

  # rubocop:disable MethodLength
  def packet_in(datapath_id, message)
    return unless sent_to_router?(message)

    case message.data
    when Arp::Request
      packet_in_arp_request datapath_id, message.in_port, message.data
    when Arp::Reply
      packet_in_arp_reply datapath_id, message
    when Parser::IPv4Packet
      packet_in_ipv4 datapath_id, message
    else
      fail "Unsupported packet_in data type: #{message.data.inspect}"
    end
  end
  # rubocop:enable MethodLength

  # rubocop:disable MethodLength
  def packet_in_arp_request(datapath_id, in_port, arp_request)
    interface =
      @interfaces.find_by(port_number: in_port,
                          ip_address: arp_request.target_protocol_address)
    return unless interface
    send_packet_out(
      datapath_id,
      raw_data: Arp::Reply.new(
        destination_mac: arp_request.source_mac,
        source_mac: interface.mac_address,
        sender_protocol_address: arp_request.target_protocol_address,
        target_protocol_address: arp_request.sender_protocol_address
      ).to_binary,
      actions: SendOutPort.new(in_port))
  end
  # rubocop:enable MethodLength

  def packet_in_arp_reply(datapath_id, message)
    @arp_table.update(message.in_port,
                      message.sender_protocol_address,
                      message.source_mac)
    maybe_send_unresolved_packets datapath_id, message.data
  end

  def packet_in_ipv4(datapath_id, message)
    if forward?(message)
      forward(datapath_id, message)
    elsif message.ip_protocol == 1
      icmp = Icmp.read(message.raw_data)
      packet_in_icmpv4_echo_request(datapath_id, message) if icmp.icmp_type == 8
    else
      fail "Failed to handle ipv4 packet: #{message.data}"
    end
  end

  # rubocop:disable AbcSize
  # rubocop:disable MethodLength
  def packet_in_icmpv4_echo_request(datapath_id, message)
    icmp_request = Icmp.read(message.raw_data)
    if @arp_table.lookup(message.ip_source_address)
      send_packet_out(datapath_id,
                      raw_data: create_icmp_reply(icmp_request).to_binary,
                      actions: SendOutPort.new(message.in_port))
    else
      @unresolved[message.ip_source_address] +=
        [[create_icmp_reply(icmp_request), [SendOutPort.new(message.in_port)]]]
      interface = @interfaces.find_by(port_number: message.in_port)
      send_arp_request(datapath_id,
                       port_number: interface.port_number,
                       source_mac: interface.mac_address,
                       source_ip: interface.ip_address,
                       destination_ip: message.ip_source_address)
    end
  end
  # rubocop:enable AbcSize
  # rubocop:enable MethodLength

  private

  def maybe_send_unresolved_packets(datapath_id, arp_reply)
    @unresolved[arp_reply.sender_protocol_address].each do |packet, actions|
      set_router_mac_actions =
        [SetEtherDestinationAddress.new(arp_reply.sender_hardware_address)] +
        actions
      send_packet_out(datapath_id,
                      raw_data: packet.to_binary_s,
                      actions: set_router_mac_actions)
    end
    @unresolved[arp_reply.sender_protocol_address] = []
  end

  def send_arp_request(datapath_id, options)
    arp_request =
      Arp::Request.new(source_mac: options.fetch(:source_mac),
                       sender_protocol_address: options.fetch(:source_ip),
                       target_protocol_address: options.fetch(:destination_ip))
    send_packet_out(datapath_id,
                    raw_data: arp_request.to_binary,
                    actions: SendOutPort.new(options.fetch(:port_number)))
  end

  def sent_to_router?(message)
    return true if message.destination_mac.broadcast?
    interface = @interfaces.find_by(port_number: message.in_port)
    interface && interface.mac_address == message.destination_mac
  end

  def forward?(message)
    !@interfaces.find_by(ip_address: message.ip_destination_address)
  end

  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  def forward(datapath_id, message)
    next_hop = resolve_next_hop(message.ip_destination_address)

    interface = @interfaces.find_by_prefix(next_hop)
    return if !interface || (interface.port_number == message.in_port)

    arp_entry = @arp_table.lookup(next_hop)
    if arp_entry
      actions = [SetEtherSourceAddress.new(interface.mac_address),
                 SetEtherDestinationAddress.new(arp_entry.mac_address),
                 SendOutPort.new(interface.port_number)]
      send_flow_mod_add(
        datapath_id,
        match: ExactMatch.new(message),
        actions: actions
      )
      send_packet_out(datapath_id,
                      in_port: message.in_port,
                      raw_data: message.raw_data,
                      actions: actions)
    else
      @unresolved[next_hop] +=
        [[message.data,
          [SetEtherSourceAddress.new(interface.mac_address),
           SendOutPort.new(interface.port_number)]]]
      send_arp_request(datapath_id,
                       port_number: interface.port_number,
                       source_mac: interface.mac_address,
                       source_ip: interface.ip_address,
                       destination_ip: next_hop)
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

  def create_icmp_reply(icmp_request)
    Icmp::Reply.new(identifier: icmp_request.icmp_identifier,
                    source_mac: icmp_request.destination_mac,
                    destination_mac: icmp_request.source_mac,
                    ip_destination_address: icmp_request.ip_source_address,
                    ip_source_address: icmp_request.ip_destination_address,
                    sequence_number: icmp_request.icmp_sequence_number,
                    echo_data: icmp_request.echo_data)
  end
end
# rubocop:enable ClassLength
