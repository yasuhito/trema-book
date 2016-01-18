module SliceExtensions
  # Extensions to Pio::PacketIn
  module PacketIn
    def slice_source
      { dpid: dpid, port_no: in_port, mac: source_mac }
    end

    def slice_destination(graph)
      graph.fetch(destination_mac).first.to_h.merge(mac: destination_mac)
    rescue KeyError
      nil
    end
  end

  # Extensions to Pio::Mac
  module Mac
    def to_json(*_)
      %({"name": "#{self}"})
    end
  end
end

module Pio
  # SliceExtensions included
  class PacketIn
    include SliceExtensions::PacketIn
  end

  # SliceExtensions included
  class Mac
    include SliceExtensions::Mac
  end
end
