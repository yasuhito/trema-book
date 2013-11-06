# -*- coding: utf-8 -*-
require 'graphviz'

module View
  #
  # Topology controller's GUI (graphviz).
  #
  class Graphviz
    def initialize(output = './topology.png')
      @output = File.expand_path(output)
    end

    def update(topology)
      graphviz = GraphViz.new(:G, use: 'neato', overlap: false, splines: true)
      nodes = add_nodes(graphviz, topology)
      add_edges(graphviz, topology, nodes)
      graphviz.output(png: @output)
    end

    private

    def add_nodes(graphviz, topology)
      switch = {}
      topology.each_switch do |dpid, ports|
        switch[dpid] = graphviz.add_nodes(dpid.to_hex, 'shape' => 'box')
      end
      switch
    end

    def add_edges(graphviz, topology, switch)
      topology.each_link do |each|
        if switch[each.dpid1] && switch[each.dpid2]
          graphviz.add_edges switch[each.dpid1], switch[each.dpid2]
        end
      end
    end
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
