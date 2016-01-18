require 'graph'
require 'path'
require 'trema'

# L2 routing path manager
class PathManager < Trema::Controller
  def start
    @graph = Graph.new
    logger.info 'Path Manager started.'
  end

  # This method smells of :reek:FeatureEnvy but ignores them
  def packet_in(_dpid, packet_in)
    path = maybe_create_shortest_path(packet_in)
    ports = path ? [path.out_port] : @graph.external_ports
    ports.each do |each|
      send_packet_out(each.dpid,
                      raw_data: packet_in.raw_data,
                      actions: SendOutPort.new(each.number))
    end
  end

  def add_port(port, _topology)
    @graph.add_link port.dpid, port
  end

  def delete_port(port, _topology)
    @graph.delete_node port
  end

  # TODO: update all paths
  def add_link(port_a, port_b, _topology)
    @graph.add_link port_a, port_b
  end

  def delete_link(port_a, port_b, _topology)
    @graph.delete_link port_a, port_b
    Path.find { |each| each.link?(port_a, port_b) }.each(&:destroy)
  end

  def add_host(mac_address, port, _topology)
    @graph.add_link mac_address, port
  end

  private

  # This method smells of :reek:FeatureEnvy but ignores them
  def maybe_create_shortest_path(packet_in)
    shortest_path =
      @graph.dijkstra(packet_in.source_mac, packet_in.destination_mac)
    return unless shortest_path
    Path.create shortest_path, packet_in
  end
end
