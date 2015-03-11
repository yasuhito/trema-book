$LOAD_PATH.unshift __dir__

require 'dijkstra'
require 'path'

# L2 routing path manager
class PathManager < Trema::Controller
  Port = Struct.new(:dpid, :number) do
    def <=>(other)
      number <=> other.number
    end

    def to_s
      "Switch #{format '%#x', dpid} (port=#{number})"
    end
  end

  def start
    @path = []
    @graph = Hash.new([].freeze)
    logger.info 'Path Manager started.'
  end

  # This method smells of :reek:FeatureEnvy but ignores them
  def packet_in(_dpid, message)
    maybe_create_shortest_path message
  end

  # This method smells of :reek:TooManyStatements but ignores them
  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  def update(event, changed, _topology)
    case event
    when :add_port, :delete_port
      of_port = changed
      __send__ event, Port.new(of_port.dpid, of_port.number)
    when :add_link, :delete_link
      link = changed
      port_a = Port.new(link.dpid_a, link.port_a)
      port_b = Port.new(link.dpid_b, link.port_b)
      __send__ event, port_a, port_b
    when :add_host
      mac_address, _ip_address, dpid, port_no = *changed
      __send__ event, mac_address, Port.new(dpid, port_no)
    when :add_switch, :delete_switch
      # ignore.
    else
      logger.debug 'Unknown event: #{event}'
    end
  end
  # rubocop:enable MethodLength
  # rubocop:enable AbcSize

  private

  def add_port(port)
    add_graph_path port.dpid, port
  end

  def delete_port(port)
    @graph.delete(port)
    @graph[port.dpid] -= [port]
  end

  def add_link(port_a, port_b)
    add_graph_path port_a, port_b
    # TODO: update all paths
  end

  def delete_link(port_a, port_b)
    delete_graph_path port_a, port_b
    paths_containing(port_a, port_b).each do |each|
      @path.delete each
      each.delete
      maybe_create_shortest_path(each.packet_in)
    end
  end

  def add_host(mac_address, port)
    add_graph_path mac_address, port
  end

  private

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
    @path << Path.create(shortest_path, packet_in)
  end

  def dijkstra(source_mac, destination_mac)
    return if @graph[destination_mac].empty?
    route = Dijkstra.new(@graph).run(source_mac, destination_mac)
    route.reject { |each| each.is_a? Integer }
  end
end
