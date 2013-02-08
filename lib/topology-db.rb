require "trema"
require "lldp-frame"


class TopologyDB
  class Link
    attr_reader :dpid1
    attr_reader :dpid2
    attr_reader :port1
    attr_reader :port2


    def initialize dpid1, port1, dpid2, port2
      @dpid1 = dpid1
      @dpid2 = dpid2
      @port1 = port1
      @port2 = port2
    end


    # FIXME
    def == other
      to_s == other.to_s
    end


    def <=> other
      to_s <=> other.to_s
    end


    def to_s
      format "%#x (port %d) <-> %#x (port %d)", dpid1, port1, dpid2, port2
    end
  end


  extend Forwardable


  def_delegators :@db, :each_pair


  def initialize
    @db = Hash.new { [].freeze }
    @links = []
  end


  def delete_switch dpid
    @db[ dpid ].each do | each |
      delete_port dpid, each
    end
    @db.delete dpid
  end


  def add_port dpid, port
    return if port.local? or port.down?
    @db[ dpid ] += [ port ]
  end


  def delete_port dpid, port
    @db[ dpid ] -= [ port ]
    @links.delete_if do | each |
      ( ( each.dpid1 == dpid ) and ( each.port1 == port.number ) ) or
        ( ( each.dpid2 == dpid ) and ( each.port2 == port.number ) )
    end
  end


  def add_link_by dpid, packet_in
    raise "Not an LLDP packet!" if not packet_in.lldp?
    lldp = Lldp.read( packet_in )
    link = Link.new( lldp.dpid, lldp.port_number, dpid, packet_in.in_port )
    @links << link if not @links.include?( link )
  end


  def to_s
    @links.sort.join "\n"
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
