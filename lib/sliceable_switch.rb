$LOAD_PATH.unshift __dir__

require 'path_manager'
require 'sliceable_switch/exceptions'
require 'sliceable_switch/slice'

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
    @slices[name] = Slice.new
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
    @slices.keys
  end

  def packet_in(_dpid, message)
    if source_and_destination_in_same_slice?(message)
      maybe_create_shortest_path_and_packet_out(message)
    else
      flood_to_external_ports(message)
    end
  end

  private

  # This method smells of :reek:TooManyStatements
  # This method smells of :reek:FeatureEnvy
  def source_and_destination_in_same_slice?(packet_in)
    source_mac = packet_in.source_mac
    destination_mac = packet_in.destination_mac
    in_port = { dpid: packet_in.dpid, port_no: packet_in.in_port }
    out_port = @graph.fetch(destination_mac).first.to_h
    @slices.values.any? do |each|
      each.has?(in_port, source_mac) && each.has?(out_port, destination_mac)
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
      next unless slice.mac_address?(packet_in.source_mac)
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
