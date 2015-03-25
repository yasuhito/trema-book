$LOAD_PATH.unshift File.join(__dir__, '..')

require 'drb'
require 'path_manager'
require 'sliceable_switch/exceptions'

# L2 routing switch with virtual slicing.
class SliceableSwitch < PathManager
  # Virtual slice.
  class Slice
    include DRb::DRbUndumped

    def initialize
      @slice = Hash.new([].freeze)
    end

    def add_port(port)
      if @slice.key?(port)
        port_name = format('%#x', port[:dpid]) + ':' + port[:port_no].to_s
        fail PortAlreadyExistsError, "Port #{port_name} already exists"
      end
      @slice[port] = [].freeze
    end

    # TODO: update paths that contains the port
    def delete_port(port)
      find_port port
      @slice.delete port
    end

    def ports
      @slice.keys
    end

    def find_port(port)
      mac_addresses port
      port
    end

    def add_mac_address(mac_address, port)
      if @slice[port].include? Pio::Mac.new(mac_address)
        fail(MacAddressAlreadyExistsError,
             "MAC address #{mac_address} already exists")
      end
      @slice[port] += [Pio::Mac.new(mac_address)]
    end

    # TODO: update paths that contains the mac address
    def delete_mac_address(mac_address, port)
      find_mac_address port, mac_address
      @slice[port] -= [Pio::Mac.new(mac_address)]
    end

    def mac_addresses(port)
      @slice.fetch(port)
    rescue KeyError
      port_name = format('%#x', port[:dpid]) + ':' + port[:port_no].to_s
      raise PortNotFoundError, "Port #{port_name} not found"
    end

    def find_mac_address(port, mac_address)
      find_port port
      if @slice[port].include? Pio::Mac.new(mac_address)
        mac_address
      else
        fail MacAddressNotFoundError, "MAC address #{mac_address} not found"
      end
    end

    def method_missing(method, *args, &block)
      @slice.__send__ method, *args, &block
    end
  end
end
