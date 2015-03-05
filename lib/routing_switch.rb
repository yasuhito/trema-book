require 'network_graph'

# L2 routing switch
class RoutingSwitch < Trema::Controller
  def start
    @path = []
    @graph = NetworkGraph.new
    logger.info 'Routing Switch started.'
  end

  def packet_in(_dpid, message)
    path = @graph.dijkstra(message.ip_source_address,
                           message.ip_destination_address)
    return unless path
    flow_mod_to_each_switch path, message
    packet_out_to_destination(*path.last, message)
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

  def add_host(ip_address, dpid, port)
    @graph.add_host(ip_address, dpid, port)
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
