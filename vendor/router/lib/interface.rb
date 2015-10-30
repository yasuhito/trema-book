require 'pio'

require 'arp_table'
require 'routing_table'

# Router network interface
class Interface
  attr_reader :mac_address
  attr_reader :ip_address
  attr_reader :masklen
  attr_reader :port

  def initialize(options)
    @port = options[:port]
    @mac_address = Pio::Mac.new(options[:mac_address])
    @ip_address = Pio::IPv4Address.new(options[:ip_address])
    @masklen = options[:masklen]
  end

  def has?(mac)
    mac == mac_address
  end
end

# List of network interfaces
class Interfaces
  def initialize(interfaces = [])
    @list = []
    interfaces.each do |each|
      @list << Interface.new(each)
    end
  end

  def find_by_port(port)
    @list.find do |each|
      each.port == port
    end
  end

  def find_by_ip_address(ip_address)
    @list.find do |each|
      each.ip_address == ip_address
    end
  end

  def find_by_prefix(ip_address)
    @list.find do |each|
      masklen = each.masklen
      each.ip_address.mask(masklen) == ip_address.mask(masklen)
    end
  end

  def find_by_port_and_ip_address(port, ip_address)
    @list.find do |each|
      each.port == port && each.ip_address == ip_address
    end
  end

  def ours?(port, macda)
    return true if macda.broadcast?

    interface = find_by_port(port)
    return true if !interface.nil? && interface.has?(macda)
  end
end
