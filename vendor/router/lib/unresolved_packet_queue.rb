require 'pio'

# Queue for packets with unresolved destination MAC address
module UnresolvedPacketQueue
  include Pio

  def send_later(datapath_id, ip_address, interface, packet)
    queue[ip_address] +=
      [[packet,
        [SetEtherSourceAddress.new(interface.mac_address),
         SendOutPort.new(interface.port_number)]]]
    send_arp_request(datapath_id,
                     port_number: interface.port_number,
                     source_mac: interface.mac_address,
                     source_ip: interface.ip_address,
                     destination_ip: ip_address)
  end

  def maybe_flush_unsent_packets(datapath_id, arp_reply)
    queue[arp_reply.sender_protocol_address].each do |packet, actions|
      set_router_mac_actions =
        [SetEtherDestinationAddress.new(arp_reply.sender_hardware_address)] +
        actions
      send_packet_out(datapath_id,
                      raw_data: packet.to_binary_s,
                      actions: set_router_mac_actions)
    end
    queue[arp_reply.sender_protocol_address] = []
  end

  private

  def queue
    @queue ||= Hash.new { [] }
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
end
