$LOAD_PATH.unshift __dir__

require 'dijkstra'
require 'path'

# L2 routing path manager
class PathManager < Trema::Controller
  def start
    @path = []
    @graph = Hash.new([].freeze)
    logger.info 'Path Manager started.'
  end

  # This method smells of :reek:FeatureEnvy but ignores them
  def packet_in(_dpid, message)
    path = maybe_create_shortest_path(message)
    ports = path ? [path.out_port] : external_ports
    ports.each do |each|
      send_packet_out(each.dpid,
                      raw_data: message.raw_data,
                      actions: SendOutPort.new(each.number))
    end
  end

  def add_port(port, _topology)
    add_graph_path port.dpid, port
  end

  def delete_port(port, _topology)
    @graph.delete(port)
    @graph[port.dpid] -= [port]
  end

  def add_link(port_a, port_b, _topology)
    add_graph_path port_a, port_b
    # TODO: update all paths
  end

  def delete_link(port_a, port_b, _topology)
    delete_graph_path port_a, port_b
    paths_containing(port_a, port_b).each do |each|
      @path.delete each
      each.delete
      maybe_create_shortest_path(each.packet_in)
    end
  end

  def add_host(mac_address, port, _topology)
    add_graph_path mac_address, port
  end

  private

  def external_ports
    @graph.select do |key, value|
      key.is_a?(Topology::Port) && value.size == 1
    end.keys
  end

  def add_graph_path(node_a, node_b)
    @graph[node_a] += [node_b]
    @graph[node_b] += [node_a]
  end

  def delete_graph_path(node_a, node_b)
    @graph[node_a] -= [node_b]
    @graph[node_b] -= [node_a]
  end

  def paths_containing(port_a, port_b)
    @path.select { |each| each.has?(port_a, port_b) }
  end

  def maybe_create_shortest_path(packet_in)
    shortest_path = dijkstra(packet_in.source_mac, packet_in.destination_mac)
    return unless shortest_path
    Path.create(shortest_path, packet_in).tap { |new_path| @path << new_path }
  end

  def dijkstra(source_mac, destination_mac)
    return if @graph[destination_mac].empty?
    route = Dijkstra.new(@graph).run(source_mac, destination_mac)
    route.reject { |each| each.is_a? Integer }
  end
end
