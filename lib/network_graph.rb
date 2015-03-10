require 'dijkstra'

# Calculates shortest path.
class NetworkGraph
  Port = Struct.new(:dpid, :number)
  Port.class_eval do
    def to_s
      "Switch #{format '%#x', dpid} (port=#{number})"
    end
  end

  def initialize
    @graph = Hash.new { [].freeze }
  end

  def dijkstra(start, goal)
    return if @graph[goal].empty?
    route = Dijkstra.new(@graph).run(start, goal)
    route.reject { |each| each.is_a? Integer }
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

  def delete_link(link)
    port_a = Port.new(link.dpid_a, link.port_a)
    port_b = Port.new(link.dpid_b, link.port_b)
    @graph[port_a] -= [port_b]
    @graph[port_b] -= [port_a]
  end

  def add_host(mac_address, dpid, port_no)
    return unless @graph[mac_address].empty?
    port = Port.new(dpid, port_no)
    @graph[mac_address] += [port]
    @graph[port] += [mac_address]
  end
end
