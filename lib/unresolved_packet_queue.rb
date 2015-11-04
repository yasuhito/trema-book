require 'pio'

# Queue for packets with unresolved destination MAC address
module UnresolvedPacketQueue
  include Pio

  def send_later(dpid, options)
    destination_ip = options.fetch(:destination_ip)
    queue[destination_ip] += [options.fetch(:data)]
    send_arp_request(dpid, destination_ip, options.fetch(:interface))
  end

  def flush_unsent_packets(dpid, arp_reply, interface)
    destination_ip = arp_reply.sender_protocol_address
    queue[destination_ip].each do |each|
      rewrite_mac =
        [SetEtherDestinationAddress.new(arp_reply.sender_hardware_address),
         SetEtherSourceAddress.new(interface.mac_address),
         SendOutPort.new(interface.port_number)]
      send_packet_out(dpid, raw_data: each.to_binary_s, actions: rewrite_mac)
    end
    queue[destination_ip] = []
  end

  private

  def queue
    @queue ||= Hash.new { [] }
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
