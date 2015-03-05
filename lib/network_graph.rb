require 'dijkstra'

# Calculates shortest path.
class NetworkGraph
  Port = Struct.new(:dpid, :number)

  def initialize
    @graph = Hash.new { [].freeze }
  end

  def dijkstra(start, goal)
    return if @graph[goal].empty?
    route = Dijkstra.new(@graph).run(start, goal)
    route.slice(1...-1).each_slice(3).map do |_in_port, dpid, out_port|
      [dpid, out_port.number]
    end
  end

  def add_port(dpid, port_no)
    port = Port.new(dpid, port_no)
    @graph[port] += [dpid]
    @graph[dpid] += [port]
  end

  # This method smells of :reek:FeatureEnvy but ignores them
  def add_link(link)
    port_a = Port.new(link.dpid_a, link.port_a)
    port_b = Port.new(link.dpid_b, link.port_b)
    @graph[port_a] += [port_b]
    @graph[port_b] += [port_a]
  end

  def add_host(ip_address, dpid, port_no)
    port = Port.new(dpid, port_no)
    @graph[ip_address] += [port]
    @graph[port] += [ip_address]
  end
end
