require 'active_support/core_ext/class/attribute_accessors'

# Routing table entry
class Route
  cattr_accessor(:all, instance_reader: false) { [] }

  def self.load_configuration(routes = [])
    self.all = routes.map { |each| new(each) }
  end

  def self.each(&block)
    all.each(&block)
  end

  include Pio

  attr_reader :destination
  attr_reader :netmask_length
  attr_reader :next_hop

  def initialize(options)
    @destination = IPv4Address.new(options.fetch(:destination))
    @netmask_length = options.fetch(:netmask_length)
    @next_hop = IPv4Address.new(options.fetch(:next_hop))
  end

  def netmask
    IPv4Address.new('255.255.255.255').mask(netmask_length)
  end
end
