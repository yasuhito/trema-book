$LOAD_PATH.unshift __dir__

require 'network_graph'
require 'path'

# L2 routing path manager
class PathManager < Trema::Controller
  def start
    @path = []
    @graph = NetworkGraph.new
    logger.info 'Path Manager started.'
  end

  def packet_in(_dpid, message)
    from, to = message.source_mac, message.destination_mac
    maybe_create_shortest_path(from, to, message)
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

  # Delete paths that contain the link.
  def delete_link(link)
    @graph.delete_link link
    @path.dup.each do |each|
      next unless each.has?(link)
      each.delete
      @path.delete each
      maybe_create_shortest_path(each.from, each.to, each.packet_in)
    end
  end

  # This method smells of :reek:LongParameterList but ignores them
  def add_host(mac_address, _ip_address, dpid, port)
    @graph.add_host(mac_address, dpid, port)
  end

  private

  def maybe_create_shortest_path(from, to, packet_in)
    shortest_path = @graph.dijkstra(from, to)
    return unless shortest_path
    @path << Path.create(shortest_path, packet_in)
  end
end
