module View
  # Topology controller's CUI.
  class Text
    def initialize(logger)
      @logger = logger
    end

    def add_switch(dpid, topology)
      show_status("Switch #{dpid.to_hex} added",
                  topology.switches.map(&:to_hex))
    end

    def delete_switch(dpid, topology)
      show_status("Switch #{dpid.to_hex} deleted",
                  topology.switches.map(&:to_hex))
    end

    def add_port(port, topology)
      add_or_delete_port :added, port, topology
    end

    def delete_port(port, topology)
      add_or_delete_port :deleted, port, topology
    end

    def add_link(port_a, port_b, topology)
      link = format('%#x-%#x', *([port_a.dpid, port_b.dpid].sort))
      show_status "Link #{link} added", topology.links
    end

    def delete_link(port_a, port_b, topology)
      link = format('%#x-%#x', *([port_a.dpid, port_b.dpid].sort))
      show_status "Link #{link} deleted", topology.links
    end

    def to_s
      'text mode'
    end

    private

    def add_or_delete_port(message, port, topology)
      ports = topology.ports[port.dpid].map(&:number).sort
      show_status "Port #{port.dpid.to_hex}:#{port.number} #{message}", ports
    end

    def show_status(message, objects)
      status = objects.sort.map(&:to_s).join(', ')
      @logger.info "#{message}: #{status}"
    end
  end
end
