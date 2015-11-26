require 'path_manager'
require 'slice_extensions'
require 'slice_exceptions'

# Virtual slice.
class Slice
  # dpid + port number
  class Port
    def self.parse(port_id)
      fail "Invalid port #{port_id}" unless /\A(0x\S+):(\d+)\Z/ =~ port_id
      { dpid: Regexp.last_match(1).hex, port_no: Regexp.last_match(2).to_i }
    end

    attr_reader :dpid
    attr_reader :port_no

    def initialize(attrs)
      @attrs = attrs
      @dpid = attrs.fetch(:dpid)
      @port_no = attrs.fetch(:port_no)
    end

    def name
      format('%#x', @dpid) + ':' + @port_no.to_s
    end

    def fetch(attr)
      @attrs.fetch attr
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

  def initialize(name)
    @name = name
    @ports = Hash.new([].freeze)
  end

  def add_port(port_attrs)
    port = Port.new(port_attrs)
    if @ports.key?(port)
      fail PortAlreadyExistsError, "Port #{port.name} already exists"
    end
    @ports[port] = [].freeze
  end

  def delete_port(port_attrs)
    find_port port_attrs
    @ports.delete Port.new(port_attrs)
  end

  def find_port(port_attrs)
    mac_addresses port_attrs
    Port.new(port_attrs)
  end

  def ports
    @ports.keys
  end

  def add_mac_address(mac_address, port_attrs)
    port = Port.new(port_attrs)
    if @ports[port].include? Pio::Mac.new(mac_address)
      fail(MacAddressAlreadyExistsError,
           "MAC address #{mac_address} already exists")
    end
    @ports[port] += [Pio::Mac.new(mac_address)]
  end

  def delete_mac_address(mac_address, port_attrs)
    find_mac_address port_attrs, mac_address
    @ports[Port.new(port_attrs)] -= [Pio::Mac.new(mac_address)]
  end

  def find_mac_address(port_attrs, mac_address)
    find_port port_attrs
    mac = Pio::Mac.new(mac_address)
    if @ports[Port.new(port_attrs)].include? mac
      mac
    else
      fail MacAddressNotFoundError, "MAC address #{mac_address} not found"
    end
  end

  def mac_addresses(port_attrs)
    port = Port.new(port_attrs)
    @ports.fetch(port)
  rescue KeyError
    raise PortNotFoundError, "Port #{port.name} not found"
  end

  def member?(host_id)
    @ports[Port.new(host_id)].include? host_id[:mac]
  end

  def to_s
    @name
  end

  def to_json(*_)
    %({"name": "#{@name}"})
  end

  def method_missing(method, *args, &block)
    @ports.__send__ method, *args, &block
  end
end
