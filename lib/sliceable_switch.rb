$LOAD_PATH.unshift __dir__

require 'pio'
require 'routing_switch'

# L2 routing switch with virtual slicing.
class SliceableSwitch < RoutingSwitch
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

  def packet_in(dpid, message)
    return unless @slices.values.any? do |each|
      each.include?(message.source_mac) &&
      each.include?(message.destination_mac)
    end
    super
  end
end
