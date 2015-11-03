require 'pio'

# Queue for packets with unresolved destination MAC address
module UnresolvedPacketQueue
  include Pio

  def send_later(datapath_id, options)
    interface = options.fetch(:interface)
    destination_ip = options.fetch(:destination_ip)
    queue[destination_ip] +=
      [[options.fetch(:data),
        [SetEtherSourceAddress.new(interface.mac_address),
         SendOutPort.new(interface.port_number)]]]
    send_arp_request(datapath_id, destination_ip, interface)
  end

  def maybe_flush_unsent_packets(datapath_id, arp_reply)
    destination_ip = arp_reply.sender_protocol_address
    queue[destination_ip].each do |packet, actions|
      set_router_mac_actions =
        [SetEtherDestinationAddress.new(arp_reply.sender_hardware_address)] +
        actions
      send_packet_out(datapath_id,
                      raw_data: packet.to_binary_s,
                      actions: set_router_mac_actions)
    end
    queue[destination_ip] = []
  end

  private

  def queue
    @queue ||= Hash.new { [] }
  end

  def send_arp_request(datapath_id, destination_ip, interface)
    arp_request =
      Arp::Request.new(source_mac: interface.mac_address,
                       sender_protocol_address: interface.ip_address,
                       target_protocol_address: destination_ip)
    send_packet_out(datapath_id,
                    raw_data: arp_request.to_binary,
                    actions: SendOutPort.new(interface.port_number))
  end
end
