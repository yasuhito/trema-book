$LOAD_PATH.unshift __dir__

require 'link'
require 'observer'
require 'graph'

# Topology information containing the list of known switches, ports,
# and links.
class Topology
  include Observable

  attr_reader :links
  attr_reader :ports

  def initialize(view)
    @ports = Hash.new { [].freeze }
    @links = []
    @fdb = {}
    @graph = Hash.new { [].freeze }
    add_observer view
  end

  def switches
    @ports.keys
  end

  def add_switch(dpid, ports)
    ports.each { |each| add_port(each) }
    changed
    notify_observers :add_switch, dpid, self
  end

  def delete_switch(dpid)
    delete_port(@ports[dpid].pop) until @ports[dpid].empty?
    @ports.delete dpid
    changed
    notify_observers :delete_switch, dpid, self
  end

  def add_port(port)
    @ports[port.dpid] += [port]
    @graph["#{port.dpid}:#{port.number}"] += [port.dpid]
    @graph[port.dpid] += ["#{port.dpid}:#{port.number}"]
    changed
    notify_observers :add_port, port, self
  end

  def delete_port(port)
    @ports[port.dpid].delete_if { |each| each.number == port.number }
    changed
    notify_observers :delete_port, port, self
    maybe_delete_link port
  end

  def maybe_add_link(link)
    return if @links.include?(link)
    @graph["#{link.dpid_a}:#{link.port_a}"] += ["#{link.dpid_b}:#{link.port_b}"]
    @graph["#{link.dpid_b}:#{link.port_b}"] += ["#{link.dpid_a}:#{link.port_a}"]
    @links << link
    changed
    notify_observers :add_link, link, self
  end

  def add_host(ip_address, dpid, port)
    @fdb[ip_address] = [dpid, port]
    @graph[ip_address] += ["#{dpid}:#{port}"]
    @graph["#{dpid}:#{port}"] += [ip_address]
  end

  def find_dpid_and_port(ip_address)
    @fdb[ip_address]
  end

  def route(ip_source_address, ip_destination_address)
    Graph.new(@graph).route(ip_source_address, ip_destination_address)
  end

  private

  def maybe_delete_link(port)
    @links.each do |each|
      next unless each.connect_to?(port)
      changed
      @links -= [each]
      notify_observers :delete_link, each, self
    end
  end
end
