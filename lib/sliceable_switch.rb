$LOAD_PATH.unshift __dir__

require 'path_manager'
require 'pio'

# L2 routing switch with virtual slicing.
class SliceableSwitch < PathManager
  def start
    super
    @slices = {}
    logger.info 'Sliceable Switch started.'
  end

  def add_slice(name)
    @slices[name] = []
  end

  def add_mac_to_slice(mac_address, slice)
    @slices[slice] << Pio::Mac.new(mac_address)
  end

  # This method smells of :reek:TooManyStatements but ignores them
  def packet_in(_dpid, message)
    return unless @slices.values.any? do |each|
      each.include?(message.source_mac) &&
      each.include?(message.destination_mac)
    end
    path = maybe_create_shortest_path(message)
    return unless path
    out_port = path.out_port
    send_packet_out(out_port.dpid,
                    packet_in: message,
                    actions: SendOutPort.new(out_port.number))
  end
end
