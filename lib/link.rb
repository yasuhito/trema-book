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


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
