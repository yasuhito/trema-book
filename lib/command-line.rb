require "rubygems"

require "gli"
require "view/text"


class CommandLine
  include GLI::App


  attr_reader :view
  attr_reader :destination_mac


  def initialize
    @view = View::Text.new
  end


  def parse argv
    program_desc "Topology discovery controller"

    flag [ :d, :destination_mac ]

    pre do | global_options, command, options, args |
      if global_options[ :destination_mac ]
        @destination_mac = Mac.new( global_options[ :destination_mac ] )
      end
      true
    end

    default_command :text

    desc "Displays topology information (text mode)"
    command :text do | c |
      c.action do | global_options, options, args |
        @view = View::Text.new
      end
    end

    desc "Displays topology information (Graphviz mode)"
    arg_name "output_file"
    command :graphviz do | c |
      c.action do | global_options, options, args |
        require "view/graphviz"
        if args.empty?
          @view = View::Graphviz.new
        else
          @view = View::Graphviz.new( args[ 0 ] )
        end
      end
    end

    run argv
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
