$LOAD_PATH.unshift __dir__

require 'path_manager'
require 'slice_extensions'
require 'sliceable_switch/exceptions'
require 'sliceable_switch/slice'

# L2 routing switch with virtual slicing.
# rubocop:disable ClassLength
class SliceableSwitch < PathManager
  attr_reader :slices

  def start
    super
    @slices = {}
    logger.info 'Sliceable Switch started.'
  end

  def add_slice(name)
    if @slices[name]
      fail SliceAlreadyExistsError, "Slice #{name} already exists"
    end
    @slices[name] = Slice.new(name)
  end

  def delete_slice(name)
    fail SliceNotFoundError, "Slice #{name} not found" unless @slices[name]
    paths_in_slice(name).each do |each|
      @path.delete each
      each.delete
    end
    @slices.delete name
  end

  def find_slice(name)
    @slices.fetch(name)
  rescue KeyError
    raise SliceNotFoundError, "Slice #{name} not found"
  end

  def slice_list
    @slices.values
  end

  def add_port_to_slice(slice_name, port_attrs)
    find_slice(slice_name).add_port(port_attrs)
  end

  def delete_port_from_slice(slice_name, port_attrs)
    find_slice(slice_name).delete_port(port_attrs)
    paths_containing_port(slice_name, port_attrs).each do |each|
      @path.delete each
      each.delete
    end
  end

  def find_port(slice_name, port_attrs)
    find_slice(slice_name).find_port(port_attrs)
  end

  def ports(slice_name)
    find_slice(slice_name).ports
  end

  def add_mac_address(slice_name, mac_address, port_attrs)
    find_slice(slice_name).add_mac_address(mac_address, port_attrs)
  end

  def delete_mac_address(slice_name, mac_address, port_attrs)
    find_slice(slice_name).delete_mac_address(mac_address, port_attrs)
    paths_containing_mac_address(slice_name,
                                 mac_address, port_attrs).each do |each|
      @path.delete each
      each.delete
    end
  end

  def find_mac_address(slice_name, port_attrs, mac_address)
    find_slice(slice_name).find_mac_address(port_attrs, mac_address)
  end

  def mac_addresses(slice_name, port_attrs)
    find_slice(slice_name).mac_addresses(port_attrs)
  end

  def packet_in(_dpid, message)
    slice_name = source_and_destination_in_same_slice?(message)
    if slice_name
      maybe_create_shortest_path_and_packet_out(slice_name, message)
    else
      flood_to_external_ports(message)
    end
  end

  private

  def paths_in_slice(slice_name)
    @path.select { |each| each.slice == slice_name }
  end

  def paths_containing_port(slice_name, port_attrs)
    paths_in_slice(slice_name).select do |each|
      each.port?(Topology::Port.create(port_attrs))
    end
  end

  def paths_containing_mac_address(slice_name, mac_address, port_attrs)
    paths_in_slice(slice_name).select do |each|
      each.endpoints.include? [Pio::Mac.new(mac_address),
                               Topology::Port.create(port_attrs)]
    end
  end

  def source_and_destination_in_same_slice?(packet_in)
    @slices.any? do |name, each|
      if each.member?(packet_in.slice_source) &&
         each.member?(packet_in.slice_destination(@graph))
        return name
      end
    end
  rescue KeyError
    false
  end

  def maybe_create_shortest_path_and_packet_out(slice_name, packet_in)
    path = maybe_create_shortest_path(packet_in)
    return unless path
    path.slice = slice_name
    out_port = path.out_port
    send_packet_out(out_port.dpid,
                    raw_data: packet_in.raw_data,
                    actions: SendOutPort.new(out_port.number))
  end

  def flood_to_external_ports(packet_in)
    @slices.values.each do |slice|
      next unless slice.member?(packet_in.slice_source)
      external_ports_in_slice(slice, packet_in.source_mac).each do |port|
        send_packet_out(port.dpid,
                        raw_data: packet_in.raw_data,
                        actions: SendOutPort.new(port.port_no))
      end
    end
  end

  def external_ports_in_slice(slice, packet_in_mac)
    slice.each_with_object([]) do |(port, macs), result|
      next unless external_ports.any? { |each| port == each }
      result << port unless macs.include?(packet_in_mac)
    end
  end
end
# rubocop:enable ClassLength
