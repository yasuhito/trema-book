# List of shortest-path flow entries.
class Path < Trema::Controller
  def self.create(shortest_path, packet_in)
    new.tap { |path| path.add(shortest_path, packet_in) }
  end

  attr_reader :packet_in

  def add(full_path, packet_in)
    @full_path = full_path
    @packet_in = packet_in
    logger.debug 'Creating path: ' + @full_path.map(&:to_s).join(' -> ')
    flow_mod_add_to_each_switch
    packet_out_to_destination
  end

  def delete
    logger.debug 'Deleting path: ' + @full_path.map(&:to_s).join(' -> ')
    flow_mod_delete_to_each_switch
  end

  def has?(*link)
    flows.any? { |each| each.sort == link.sort }
  end

  private

  def flows
    path[1..-2].each_slice(2).to_a
  end

  def flow_mod_add_to_each_switch
    path.each_slice(2) do |in_port, out_port|
      send_flow_mod_add(out_port.dpid,
                        match: exact_match(in_port.number),
                        actions: SendOutPort.new(out_port.number))
    end
  end

  def flow_mod_delete_to_each_switch
    path.each_slice(2) do |in_port, out_port|
      send_flow_mod_delete(out_port.dpid,
                           match: exact_match(in_port.number),
                           out_port: out_port.number)
    end
  end

  # rubocop:disable AbcSize
  # rubocop:disable MethodLength
  def exact_match(in_port)
    match_options = {
      in_port: in_port,
      dl_src: @packet_in.source_mac,
      dl_dst: @packet_in.destination_mac,
      dl_vlan: @packet_in.data.vlan_vid,
      dl_vlan_pcp: @packet_in.data.vlan_pcp,
      dl_type: @packet_in.data.ether_type,
      nw_tos: @packet_in.data.ip_type_of_service,
      nw_proto: @packet_in.data.ip_protocol,
      nw_src: @packet_in.data.ip_source_address,
      nw_dst: @packet_in.data.ip_destination_address,
      tp_src: @packet_in.data.transport_source_port,
      tp_dst: @packet_in.data.transport_destination_port
    }
    Match.new(match_options)
  end
  # rubocop:enable AbcSize
  # rubocop:enable MethodLength

  def packet_out_to_destination
    out_port = path.last
    send_packet_out(out_port.dpid,
                    packet_in: @packet_in,
                    actions: SendOutPort.new(out_port.number))
  end

  def path
    @full_path[1..-2]
  end
end
