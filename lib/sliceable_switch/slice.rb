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
      fail PortAlreadyExistsError if @slice.key?(port)
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
      @slice.fetch(port)
      port
    rescue KeyError
      raise PortNotFoundError
    end

    def add_mac_address(mac_address, port)
      if mac_address_exists?(port, mac_address)
        fail MacAddressAlreadyExistsError
      end
      @slice[port] += [Pio::Mac.new(mac_address)]
    end

    # TODO: update paths that contains the mac address
    def delete_mac_address(mac_address, port)
      find_port port
      fail MacAddressNotFoundError unless mac_address_exists?(port, mac_address)
      @slice[port] -= [Pio::Mac.new(mac_address)]
    end

    def mac_addresses(port)
      @slice.fetch(port)
    rescue KeyError
      raise PortNotFoundError
    end

    def method_missing(method, *args, &block)
      @slice.__send__ method, *args, &block
    end

    private

    def mac_address_exists?(port, mac_address)
      @slice[port].include? Pio::Mac.new(mac_address)
    end
  end
end
