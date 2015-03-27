$LOAD_PATH.unshift __dir__

require 'path_manager'
require 'sliceable_switch/exceptions'
require 'sliceable_switch/slice'

module Pio
  # Adds #slice_source
  class PacketIn
    def slice_source
      { dpid: dpid, port_no: in_port, mac: source_mac }
    end

    def slice_destination(topology)
      topology.fetch(destination_mac).first.to_h.merge(mac: destination_mac)
    end
  end
end

# L2 routing switch with virtual slicing.
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

  # TODO: delete all paths in the slice
  def delete_slice(name)
    fail SliceNotFoundError, "Slice #{name} not found" unless @slices[name]
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

  # TODO: update paths that contains the port
  def delete_port_from_slice(slice_name, port_attrs)
    find_slice(slice_name).delete_port(port_attrs)
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

  # TODO: update paths that contains the mac address
  def delete_mac_address(slice_name, mac_address, port_attrs)
    find_slice(slice_name).delete_mac_address(mac_address, port_attrs)
  end

  def find_mac_address(slice_name, port_attrs, mac_address)
    find_slice(slice_name).find_mac_address(port_attrs, mac_address)
  end

  def mac_addresses(slice_name, port_attrs)
    find_slice(slice_name).mac_addresses(port_attrs)
  end

  def packet_in(_dpid, message)
    if source_and_destination_in_same_slice?(message)
      maybe_create_shortest_path_and_packet_out(message)
    else
      flood_to_external_ports(message)
    end
  end

  private

  def source_and_destination_in_same_slice?(packet_in)
    @slices.values.any? do |each|
      each.member?(packet_in.slice_source) &&
        each.member?(packet_in.slice_destination(@graph))
    end
  rescue KeyError
    false
  end

  def maybe_create_shortest_path_and_packet_out(packet_in)
    path = maybe_create_shortest_path(packet_in)
    return unless path
    out_port = path.out_port
    send_packet_out(out_port.dpid,
                    raw_data: packet_in.raw_data,
                    actions: SendOutPort.new(out_port.number))
  end

  # rubocop:disable MethodLength
  # rubocop:disable AbcSize
  def flood_to_external_ports(packet_in)
    @slices.values.each do |slice|
      next unless slice.member?(packet_in.slice_source)
      slice.each do |port, macs|
        next unless external_ports.any? { |each| port == each }
        next if macs.include?(packet_in.source_mac)
        send_packet_out(port.dpid,
                        raw_data: packet_in.raw_data,
                        actions: SendOutPort.new(port.port_no))
      end
    end
  end
  # rubocop:enable MethodLength
  # rubocop:enable AbcSize
end
