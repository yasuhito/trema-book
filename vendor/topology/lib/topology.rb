require 'link'

# Topology information containing the list of known switches, ports,
# and links.
class Topology
  Port = Struct.new(:dpid, :port_no) do
    alias_method :number, :port_no

    def self.create(attrs)
      new attrs.fetch(:dpid), attrs.fetch(:port_no)
    end

    def <=>(other)
      [dpid, number] <=> [other.dpid, other.number]
    end

    def to_s
      "#{format '%#x', dpid}:#{number}"
    end
  end

  attr_reader :links
  attr_reader :ports

  def initialize
    @observers = []
    @ports = Hash.new { [].freeze }
    @links = []
    @hosts = []
  end

  def add_observer(observer)
    @observers << observer
  end

  def switches
    @ports.keys
  end

  def add_switch(dpid, ports)
    ports.each { |each| add_port(each) }
    maybe_send_handler :add_switch, dpid, self
  end

  def delete_switch(dpid)
    delete_port(@ports[dpid].pop) until @ports[dpid].empty?
    @ports.delete dpid
    maybe_send_handler :delete_switch, dpid, self
  end

  def add_port(port)
    @ports[port.dpid] += [port]
    maybe_send_handler :add_port, Port.new(port.dpid, port.number), self
  end

  def delete_port(port)
    @ports[port.dpid].delete_if { |each| each.number == port.number }
    maybe_send_handler :delete_port, Port.new(port.dpid, port.number), self
    maybe_delete_link port
  end

  def maybe_add_link(link)
    return if @links.include?(link)
    @links << link
    port_a = Port.new(link.dpid_a, link.port_a)
    port_b = Port.new(link.dpid_b, link.port_b)
    maybe_send_handler :add_link, port_a, port_b, self
  end

  def maybe_add_host(*host)
    return if @hosts.include?(host)
    @hosts << host
    mac_address, _ip_address, dpid, port_no = *host
    maybe_send_handler :add_host, mac_address, Port.new(dpid, port_no), self
  end

  def route(ip_source_address, ip_destination_address)
    @graph.route(ip_source_address, ip_destination_address)
  end

  private

  def maybe_delete_link(port)
    @links.each do |each|
      next unless each.connect_to?(port)
      @links -= [each]
      port_a = Port.new(each.dpid_a, each.port_a)
      port_b = Port.new(each.dpid_b, each.port_b)
      maybe_send_handler :delete_link, port_a, port_b, self
    end
  end

  def maybe_send_handler(method, *args)
    @observers.each do |each|
      if each.respond_to?(:update)
        each.__send__ :update, method, args[0..-2], args.last
      end
      each.__send__ method, *args if each.respond_to?(method)
    end
  end
end
