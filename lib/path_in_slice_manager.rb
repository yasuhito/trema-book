require 'path_manager'
require 'slice'
require 'slice_exceptions'
require 'slice_extensions'

# L2 routing switch with virtual slicing.
class PathInSliceManager < PathManager
  def start
    super
    logger.info "#{name} started."
  end

  # rubocop:disable MethodLength
  def packet_in(_dpid, packet_in)
    slice = Slice.find do |each|
      each.member?(packet_in.slice_source) &&
      each.member?(packet_in.slice_destination(@graph))
    end
    ports = if slice
              path = maybe_create_shortest_path_in_slice(slice.name, packet_in)
              path ? [path.out_port] : []
            else
              external_ports(packet_in)
            end
    packet_out(packet_in.raw_data, ports)
  end
  # rubocop:enable MethodLength

  private

  def packet_out(raw_data, ports)
    ports.each do |each|
      send_packet_out(each.dpid,
                      raw_data: raw_data,
                      actions: SendOutPort.new(each.port_no))
    end
  end

  def maybe_create_shortest_path_in_slice(slice_name, packet_in)
    path = maybe_create_shortest_path(packet_in)
    return unless path
    path.slice = slice_name
    path
  end

  def external_ports(packet_in)
    Slice.all.each_with_object([]) do |each, ports|
      next unless each.member?(packet_in.slice_source)
      ports.concat external_ports_in_slice(each, packet_in.source_mac)
    end
  end

  def external_ports_in_slice(slice, packet_in_mac)
    slice.each_with_object([]) do |(port, macs), result|
      next unless @graph.external_ports.any? { |each| port == each }
      result << port unless macs.include?(packet_in_mac)
    end
  end
end
