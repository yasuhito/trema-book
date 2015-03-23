$LOAD_PATH.unshift __dir__

require 'path_manager'

# L2 routing switch with virtual slicing.
class SliceableSwitch < PathManager
  # Slice not found.
  class SliceNotFoundError < StandardError; end
  # Port not found.
  class PortNotFoundError < StandardError; end

  attr_reader :slices

  def start
    super
    @slices = {}
    logger.info 'Sliceable Switch started.'
  end

  def add_slice(name)
    fail "Slice named #{name} already exists." if @slices[name]
    @slices[name] = Hash.new([].freeze)
  end

  # TODO: delete all paths in the slice
  def delete_slice(name)
    unless @slices[name]
      fail SliceNotFoundError, "Slice named #{name} does not exist."
    end
    @slices.delete name
  end

  def find_slice(name)
    @slices.fetch(name)
  rescue KeyError
    raise SliceNotFoundError, "Slice named #{name} does not exist."
  end

  def slice_list
    @slices.keys
  end

  def find_port(slice_name, port)
    find_slice(slice_name).fetch(port)
    port
  rescue KeyError
    raise PortNotFoundError
  end

  def ports(slice_name)
    find_slice(slice_name).keys
  end

  def add_port_to_slice(slice_name, port)
    find_slice(slice_name)[port] = []
  end

  # TODO: update paths that contains the port
  def delete_port_from_slice(slice_name, port)
    find_slice(slice_name).delete port
  end

  def mac_addresses(slice_name, port)
    find_slice(slice_name).fetch(port)
  rescue KeyError
    raise PortNotFoundError
  end

  def add_mac_address_to_slice(mac_address, slice_name, port)
    find_slice(slice_name)[port] += [Pio::Mac.new(mac_address)]
  end

  # TODO: update paths that contains the mac address
  def delete_mac_address_from_slice(mac_address, slice_name, port)
    find_slice(slice_name)[port] -= [Pio::Mac.new(mac_address)]
  end

  def packet_in(dpid, message)
    if source_and_destination_in_same_slice?(dpid, message)
      maybe_create_shortest_path_and_packet_out(message)
    else
      flood_to_external_ports(message)
    end
  end

  private

  def source_and_destination_in_same_slice?(in_dpid, packet_in)
    in_port = { dpid: in_dpid, port_no: packet_in.in_port }
    out_port = @graph.fetch(packet_in.destination_mac).first.to_h
    @slices.values.any? do |each|
      each[in_port].include?(packet_in.source_mac) &&
        each[out_port].include?(packet_in.destination_mac)
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
      next unless slice.values.any? do |macs|
        macs.include?(packet_in.source_mac)
      end
      slice.each do |port, macs|
        next unless external_ports.any? do |each|
          each.dpid == port.fetch(:dpid) && each.port_no == port.fetch(:port_no)
        end
        next if macs.include?(packet_in.source_mac)
        send_packet_out(port.fetch(:dpid),
                        raw_data: packet_in.raw_data,
                        actions: SendOutPort.new(port.fetch(:port_no)))
      end
    end
  end
  # rubocop:enable MethodLength
  # rubocop:enable AbcSize
end
