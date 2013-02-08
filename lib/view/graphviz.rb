require "graphviz"


module View
  class Graphviz
    def initialize output = "~/Dropbox/topology.png"
      @output = File.expand_path( output )
    end


    def update topology
      g = GraphViz.new( :G, :use => "neato" )

      switch = {}
      topology.each_switch do | dpid, ports |
        switch[ dpid ] = g.add_nodes( format( "%#x", dpid ), "shape" => "box" )
      end

      topology.each_link do | each |
        g.add_edges switch[ each.dpid1 ], switch[ each.dpid2 ]
      end

      g.output( :png => @output )
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
