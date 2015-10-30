# ARP table
class ArpTable
  # ARP table entry
  class ArpEntry
    attr_reader :port_number
    attr_reader :mac_address

    def initialize(port_number, mac_address)
      @port_number = port_number
      @mac_address = mac_address
    end
  end

  def initialize
    @db = {}
  end

  def update(port_number, ip_address, mac_address)
    return if @db[ip_address.to_i]
    @db[ip_address.to_i] = ArpEntry.new(port_number, mac_address)
  end

  def lookup(ip_address)
    @db[ip_address.to_i]
  end
end
