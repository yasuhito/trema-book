require 'active_support/core_ext/class/attribute_accessors'

# Router network interface
class Interface
  cattr_accessor(:all, instance_reader: false) { [] }

  def self.load_configuration(configuration)
    self.all = configuration.map { |each| new(each) }
  end

  def self.find_by(queries)
    queries.inject(all) do |memo, (attr, value)|
      memo.find_all do |interface|
        interface.__send__(attr) == value
      end
    end.first
  end

  def self.find_by_prefix(ip_address)
    all.find do |each|
      netmask_length = each.netmask_length
      each.ip_address.mask(netmask_length) == ip_address.mask(netmask_length)
    end
  end

  def self.each(&block)
    all.each(&block)
  end

  include Pio

  attr_reader :mac_address
  attr_reader :ip_address
  attr_reader :netmask_length
  attr_reader :port_number

  def initialize(options)
    @port_number = options.fetch(:port)
    @mac_address = Mac.new(options.fetch(:mac_address))
    @ip_address = IPv4Address.new(options.fetch(:ip_address))
    @netmask_length = options.fetch(:netmask_length)
  end

  def network_address
    ip_address.mask netmask_length
  end

  def netmask
    IPv4Address.new('255.255.255.255').mask(netmask_length)
  end
end
