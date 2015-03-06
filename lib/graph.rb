# Run dijkstra on network topology.
class Graph
  # Graph node
  class Node
    attr_reader :name
    attr_reader :neighbors
    attr_accessor :distance
    attr_accessor :prev
    def initialize(name, neighbors)
      @name = name
      @neighbors = neighbors
      @distance = Float::INFINITY
      @prev = nil
      @visited = false
    end

    def maybe_update_distance(from, new_distance)
      return if @distance && new_distance > @distance
      @distance = new_distance
      @prev = from.name
    end

    def visit
      @visited = true
      self
    end

    def unvisited?
      !@visited
    end
  end

  def initialize
    @data = Hash.new { [].freeze }
  end

  def update(event, changed, _topology)
    case event
    when :add_port, :add_link
      __send__ event, changed
    when :add_host
      __send__ event, *changed
    else
      true
    end
  end

  # rubocop:disable MethodLength
  def route(start, goal)
    @nodes = @data.each_with_object({}) do |(node, neighbors), hash|
      hash[node] = Node.new(node, neighbors)
    end

    return unless @nodes[start]
    return unless @nodes[goal]

    dijkstra start
    tmp = @nodes.fetch(goal)
    result = [tmp.name]
    while tmp.prev
      tmp = @nodes.fetch(tmp.prev)
      result.unshift tmp.name
    end
    result
  rescue KeyError
    nil
  end
  # rubocop:enable MethodLength

  private

  def add_port(port)
    @data["#{port.dpid}:#{port.number}"] += [port.dpid]
    @data[port.dpid] += ["#{port.dpid}:#{port.number}"]
  end

  def add_link(link)
    @data["#{link.dpid_a}:#{link.port_a}"] += ["#{link.dpid_b}:#{link.port_b}"]
    @data["#{link.dpid_b}:#{link.port_b}"] += ["#{link.dpid_a}:#{link.port_a}"]
  end

  def add_host(ip_address, dpid, port)
    @data[ip_address] += ["#{dpid}:#{port}"]
    @data["#{dpid}:#{port}"] += [ip_address]
  end

  def dijkstra(start_name)
    @nodes.fetch(start_name).distance = 0
    loop do
      min_node = find_unvisited_min_distance_node
      break unless min_node
      min_node.neighbors.each do |each|
        @nodes.fetch(each).
          maybe_update_distance(min_node, min_node.distance + 1)
      end
      min_node.visit
    end
  end

  def find_unvisited_min_distance_node
    found = nil
    @nodes.values.select(&:unvisited?).each do |each|
      found = each if found.nil? || each.distance < found.distance
    end
    found
  end
end
