# Finds shortest path.
class Dijkstra
  # Graph node
  class Node
    attr_reader :name
    attr_reader :neighbors
    attr_reader :distance
    attr_reader :prev

    def initialize(name, neighbors)
      @name = name
      @neighbors = neighbors
      @distance = 100_000
      @prev = nil
    end

    def maybe_update_distance_and_prev(min_node)
      new_distance = min_node.distance + 1
      return if new_distance > @distance
      @distance = new_distance
      @prev = min_node
    end

    def distance=(new_distance)
      fail if new_distance < 0
      @distance = new_distance
    end

    def <=>(other)
      @distance <=> other.distance
    end
  end

  # Sorted list.
  # TODO: Replace with heap.
  class SortedArray
    def initialize(array)
      @array = []
      array.each { |each| @array << each }
      @array.sort!
    end

    def method_missing(method, *args)
      result = @array.__send__ method, *args
      @array.sort!
      result
    end
  end

  def initialize(graph)
    @all = graph.map { |name, neighbors| Node.new(name, neighbors) }
    @unvisited = SortedArray.new(@all)
  end

  def run(start, goal)
    find(start, @all).distance = 0
    maybe_update_neighbors_of(@unvisited.shift) until @unvisited.empty?
    result = path_to(goal)
    result.include?(start) ? result : []
  end

  private

  def maybe_update_neighbors_of(min_node)
    min_node.neighbors.each do |each|
      find(each, @all).maybe_update_distance_and_prev(min_node)
    end
  end

  # This method smells of :reek:FeatureEnvy but ignores them
  # This method smells of :reek:DuplicateMethodCall but ignores them
  def path_to(goal)
    [find(goal, @all)].tap do |result|
      result.unshift result.first.prev while result.first.prev
    end.map(&:name)
  end

  def find(name, list)
    found = list.find { |each| each.name == name }
    fail "Node #{name.inspect} not found" unless found
    found
  end
end
