require 'arp_table'
require 'interface'
require 'routing_table'

# Simple implementation of L3 switch in OpenFlow1.0
# rubocop:disable ClassLength
class SimpleRouter < Trema::Controller
  def start(_args)
    load File.join(__dir__, '..', 'simple_router.conf')
    Interface.load_configuration Configuration::INTERFACES
    @arp_table = ArpTable.new
    @routing_table = RoutingTable.new(Configuration::ROUTES)
    @unresolved_packet_queue = Hash.new { [] }
    logger.info "#{name} started."
  end

  def switch_ready(dpid)
    send_flow_mod_delete(dpid, match: Match.new)
  end

  # rubocop:disable MethodLength
  def packet_in(dpid, packet_in)
    return unless sent_to_router?(packet_in)

    case packet_in.data
    when Arp::Request
      packet_in_arp_request dpid, packet_in.in_port, packet_in.data
    when Arp::Reply
      packet_in_arp_reply dpid, packet_in
    when Parser::IPv4Packet
      packet_in_ipv4 dpid, packet_in
    else
      logger.debug "Dropping unsupported packet type: #{packet_in.data.inspect}"
    end
  end
  # rubocop:enable MethodLength

  # rubocop:disable MethodLength
  def packet_in_arp_request(dpid, in_port, arp_request)
    interface =
      Interface.find_by(port_number: in_port,
                        ip_address: arp_request.target_protocol_address)
    return unless interface
    send_packet_out(
      dpid,
      raw_data: Arp::Reply.new(
        destination_mac: arp_request.source_mac,
        source_mac: interface.mac_address,
        sender_protocol_address: arp_request.target_protocol_address,
        target_protocol_address: arp_request.sender_protocol_address
      ).to_binary,
      actions: SendOutPort.new(in_port))
  end
  # rubocop:enable MethodLength

  def packet_in_arp_reply(dpid, packet_in)
    @arp_table.update(packet_in.in_port,
                      packet_in.sender_protocol_address,
                      packet_in.source_mac)
    flush_unsent_packets(dpid,
                         packet_in.data,
                         Interface.find_by(port_number: packet_in.in_port))
  end

  def packet_in_ipv4(dpid, packet_in)
    if forward?(packet_in)
      forward(dpid, packet_in)
    elsif packet_in.ip_protocol == 1
      icmp = Icmp.read(packet_in.raw_data)
      packet_in_icmpv4_echo_request(dpid, packet_in) if icmp.icmp_type == 8
    else
      logger.debug "Dropping unsupported IPv4 packet: #{packet_in.data}"
    end
  end

  # rubocop:disable MethodLength
  def packet_in_icmpv4_echo_request(dpid, packet_in)
    icmp_request = Icmp.read(packet_in.raw_data)
    if @arp_table.lookup(packet_in.source_ip_address)
      send_packet_out(dpid,
                      raw_data: create_icmp_reply(icmp_request).to_binary,
                      actions: SendOutPort.new(packet_in.in_port))
    else
      send_later(dpid,
                 interface: Interface.find_by(port_number: packet_in.in_port),
                 destination_ip: packet_in.source_ip_address,
                 data: create_icmp_reply(icmp_request))
    end
  end
  # rubocop:enable MethodLength

  private

  def sent_to_router?(packet_in)
    return true if packet_in.destination_mac.broadcast?
    interface = Interface.find_by(port_number: packet_in.in_port)
    interface && interface.mac_address == packet_in.destination_mac
  end

  def forward?(packet_in)
    !Interface.find_by(ip_address: packet_in.destination_ip_address)
  end

  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  def forward(dpid, packet_in)
    next_hop = resolve_next_hop(packet_in.destination_ip_address)

    interface = Interface.find_by_prefix(next_hop)
    return if !interface || (interface.port_number == packet_in.in_port)

    arp_entry = @arp_table.lookup(next_hop)
    if arp_entry
      actions = [SetSourceMacAddress.new(interface.mac_address),
                 SetDestinationMacAddress.new(arp_entry.mac_address),
                 SendOutPort.new(interface.port_number)]
      send_flow_mod_add(dpid,
                        match: ExactMatch.new(packet_in), actions: actions)
      send_packet_out(dpid, raw_data: packet_in.raw_data, actions: actions)
    else
      send_later(dpid,
                 interface: interface,
                 destination_ip: next_hop,
                 data: packet_in.data)
    end
  end
  # rubocop:enable AbcSize
  # rubocop:enable MethodLength

  def resolve_next_hop(destination_ip_address)
    interface = Interface.find_by_prefix(destination_ip_address)
    if interface
      destination_ip_address
    else
      @routing_table.lookup(destination_ip_address)
    end
  end

  def create_icmp_reply(icmp_request)
    Icmp::Reply.new(identifier: icmp_request.icmp_identifier,
                    source_mac: icmp_request.destination_mac,
                    destination_mac: icmp_request.source_mac,
                    destination_ip_address: icmp_request.source_ip_address,
                    source_ip_address: icmp_request.destination_ip_address,
                    sequence_number: icmp_request.icmp_sequence_number,
                    echo_data: icmp_request.echo_data)
  end

  def send_later(dpid, options)
    destination_ip = options.fetch(:destination_ip)
    @unresolved_packet_queue[destination_ip] += [options.fetch(:data)]
    send_arp_request(dpid, destination_ip, options.fetch(:interface))
  end

  def flush_unsent_packets(dpid, arp_reply, interface)
    destination_ip = arp_reply.sender_protocol_address
    @unresolved_packet_queue[destination_ip].each do |each|
      rewrite_mac =
        [SetDestinationMacAddress.new(arp_reply.sender_hardware_address),
         SetSourceMacAddress.new(interface.mac_address),
         SendOutPort.new(interface.port_number)]
      send_packet_out(dpid, raw_data: each.to_binary_s, actions: rewrite_mac)
    end
    @unresolved_packet_queue[destination_ip] = []
  end

  def send_arp_request(dpid, destination_ip, interface)
    arp_request =
      Arp::Request.new(source_mac: interface.mac_address,
                       sender_protocol_address: interface.ip_address,
                       target_protocol_address: destination_ip)
    send_packet_out(dpid,
                    raw_data: arp_request.to_binary,
                    actions: SendOutPort.new(interface.port_number))
  end
end
# rubocop:enable ClassLength
