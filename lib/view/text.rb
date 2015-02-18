module View
  # Topology controller's CUI.
  class Text
    def initialize(logger)
      @logger = logger
    end

    def update(event, changed, topology)
      __send__ event, changed, topology
    end

    private

    def add_switch(dpid, topology)
      show_status("Switch #{dpid.to_hex} added",
                  topology.switches.map(&:to_hex))
    end

    def delete_switch(dpid, topology)
      show_status("Switch #{dpid.to_hex} deleted",
                  topology.switches.map(&:to_hex))
    end

    def add_port(port, _topology)
      show_status "Port #{port.dpid.to_hex}:#{port.number} added", []
    end

    def delete_port(port, _topology)
      show_status "Port #{port.dpid.to_hex}:#{port.number} deleted", []
    end

    def add_link(link, topology)
      show_status "Link #{link} added", topology.links
    end

    def delete_link(link, topology)
      show_status "Link #{link} deleted", topology.links
    end

    def show_status(message, objects)
      status = objects.sort.map(&:to_s).join(', ')
      @logger.info "#{message}: #{status}"
    end
  end
end
