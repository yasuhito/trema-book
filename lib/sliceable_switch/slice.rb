$LOAD_PATH.unshift File.join(__dir__, '..')

require 'drb'
require 'path_manager'
require 'sliceable_switch/exceptions'

# L2 routing switch with virtual slicing.
class SliceableSwitch < PathManager
  # Virtual slice.
  class Slice
    # dpid + port number
    class Port
      attr_reader :dpid
      attr_reader :port_no

      def initialize(attrs)
        @dpid = attrs.fetch(:dpid)
        @port_no = attrs.fetch(:port_no)
      end

      def name
        format('%#x', @dpid) + ':' + @port_no.to_s
      end

      def to_json(*_)
        %({"name": "#{name}", "dpid": #{@dpid}, "port_no": #{@port_no}})
      end

      def ==(other)
        eql? other
      end

      def eql?(other)
        @dpid == other.dpid && @port_no == other.port_no
      end

      def hash
        name.hash
      end
    end

    include DRb::DRbUndumped

    def initialize
      @slice = Hash.new([].freeze)
    end

    def add_port(port_attrs)
      port = Port.new(port_attrs)
      if @slice.key?(port)
        fail PortAlreadyExistsError, "Port #{port.name} already exists"
      end
      @slice[port] = [].freeze
    end

    # TODO: update paths that contains the port
    def delete_port(port_attrs)
      find_port port_attrs
      @slice.delete Port.new(port_attrs)
    end

    def ports
      @slice.keys
    end

    def find_port(port_attrs)
      mac_addresses port_attrs
      Port.new(port_attrs)
    end

    def add_mac_address(mac_address, port_attrs)
      port = Port.new(port_attrs)
      if @slice[port].include? Pio::Mac.new(mac_address)
        fail(MacAddressAlreadyExistsError,
             "MAC address #{mac_address} already exists")
      end
      @slice[port] += [Pio::Mac.new(mac_address)]
    end

    # TODO: update paths that contains the mac address
    def delete_mac_address(mac_address, port_attrs)
      find_mac_address port_attrs, mac_address
      @slice[Port.new(port_attrs)] -= [Pio::Mac.new(mac_address)]
    end

    def mac_addresses(port_attrs)
      port = Port.new(port_attrs)
      @slice.fetch(port)
    rescue KeyError
      raise PortNotFoundError, "Port #{port.name} not found"
    end

    def find_mac_address(port_attrs, mac_address)
      find_port port_attrs
      if @slice[Port.new(port_attrs)].include? Pio::Mac.new(mac_address)
        mac_address
      else
        fail MacAddressNotFoundError, "MAC address #{mac_address} not found"
      end
    end

    def has?(port_attrs, mac)
      @slice[Port.new(port_attrs)].include? mac
    end

    def mac_address?(mac)
      @slice.values.any? { |each| each.include? mac }
    end

    def method_missing(method, *args, &block)
      @slice.__send__ method, *args, &block
    end
  end
end
