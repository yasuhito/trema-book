# -*- coding: utf-8 -*-
require 'rubygems'

require 'gli'
require 'view/text'

#
# command-line options passed to topology-controller.rb
#
class CommandLine
  include GLI::App

  attr_reader :view
  attr_reader :destination_mac

  def initialize
    @view = View::Text.new
  end

  def parse(argv)
    program_desc 'Topology discovery controller'
    set_destination_mac_flag
    define_text_command
    define_graphviz_command
    run argv
  end

  private

  def set_destination_mac_flag
    flag [:d, :destination_mac]
    pre do |global_options, command, options, args|
      destination_mac = global_options[:destination_mac]
      @destination_mac = Mac.new(destination_mac) if destination_mac
      true
    end
  end

  def define_text_command
    default_command :text
    desc 'Displays topology information (text mode)'
    command :text do |cmd|
      cmd.action(&method(:create_text_view))
    end
  end

  def define_graphviz_command
    desc 'Displays topology information (Graphviz mode)'
    arg_name 'output_file'
    command :graphviz do |cmd|
      cmd.action(&method(:create_graphviz_view))
    end
  end

  private

  def create_text_view(_global_options, _options, _args)
    @view = View::Text.new
  end

  def create_graphviz_view(_global_options, _options, args)
    require 'view/graphviz'
    if args.empty?
      @view = View::Graphviz.new
    else
      @view = View::Graphviz.new(args[0])
    end
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
