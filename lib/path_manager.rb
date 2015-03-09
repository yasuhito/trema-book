$LOAD_PATH.unshift __dir__

require 'network_graph'

# L2 routing switch
class PathManager < Trema::Controller
  def start
    @graph = NetworkGraph.new
    logger.info 'Path Manager started.'
  end

  def packet_in(_dpid, message)
    shortest_path = @graph.dijkstra(message.source_mac, message.destination_mac)
    return unless shortest_path
    add_path(message.source_mac,
             shortest_path,
             message.destination_mac,
             message)
  end

  def update(event, changed, _topology)
    case event
    when :add_port, :add_link, :delete_link, :delete_port
      __send__ event, changed
    when :add_host
      __send__ event, *changed
    when :add_switch, :delete_switch
      # ignore.
    else
      logger.debug 'Unknown event: #{event}'
    end
  end

  private

  # This method smells of :reek:FeatureEnvy but ignores them
  def add_port(port)
    @graph.add_port port.dpid, port.number
  end

  def delete_port(_port)
    # TODO: delete paths that contain the port.
    true
  end

  def add_link(link)
    @graph.add_link link
    # TODO: update all paths
  end

  def delete_link(_link)
    # TODO: delete paths that contain the link.
  end

  # This method smells of :reek:LongParameterList but ignores them
  def add_host(mac_address, _ip_address, dpid, port)
    @graph.add_host(mac_address, dpid, port)
  end

  def add_path(_from, shortest_path, _to, packet_in)
    shortest_path = @graph.dijkstra(packet_in.source_mac,
                                    packet_in.destination_mac)
    return unless shortest_path
    flow_mod_to_each_switch shortest_path, packet_in
    packet_out_to_destination(*shortest_path.last, packet_in)
  end

  def flow_mod_to_each_switch(path, packet_in)
    path.each do |dpid, port_no|
      send_flow_mod_add(dpid,
                        match: ExactMatch.new(packet_in),
                        actions: SendOutPort.new(port_no))
    end
  end

  def packet_out_to_destination(dpid, port_no, packet_in)
    send_packet_out(dpid,
                    packet_in: packet_in,
                    actions: SendOutPort.new(port_no))
  end
end
