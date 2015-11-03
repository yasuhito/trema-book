# Routing table
class RoutingTable
  ADDR_LEN = 32

  def initialize(route = [])
    @db = Array.new(ADDR_LEN + 1) { Hash.new }
    route.each do |each|
      add(each)
    end
  end

  def add(options)
    dest = IPAddr.new(options.fetch(:destination))
    mask_length = options.fetch(:mask_length)
    prefix = dest.mask(mask_length)
    @db[mask_length][prefix.to_i] = IPAddr.new(options.fetch(:next_hop))
  end

  def delete(options)
    dest = IPAddr.new(options.fetch(:destination))
    mask_length = options.fetch(:mask_length)
    prefix = dest.mask(mask_length)
    @db[mask_length].delete(prefix.to_i)
  end

  def lookup(dest)
    (0..ADDR_LEN).reverse_each do |mask_length|
      prefix = dest.mask(mask_length)
      entry = @db[mask_length][prefix.to_i]
      return entry if entry
    end
    nil
  end
end
