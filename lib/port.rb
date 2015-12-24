# dpid + port number
class Port
  def self.parse(port_id)
    fail "Invalid port #{port_id}" unless /\A(0x\S+):(\d+)\Z/ =~ port_id
    { dpid: Regexp.last_match(1).hex, port_no: Regexp.last_match(2).to_i }
  end

  attr_reader :dpid
  attr_reader :port_no

  def initialize(attrs)
    @attrs = attrs
    @dpid = attrs.fetch(:dpid)
    @port_no = attrs.fetch(:port_no)
  end

  def name
    format('%#x', @dpid) + ':' + @port_no.to_s
  end

  def fetch(attr)
    @attrs.fetch attr
  end

  def to_json(*_)
    %({"name": "#{name}", "dpid": #{@dpid}, "port_no": #{@port_no}})
  end

  def ==(other)
    eql? other
  end

  def eql?(other)
    @dpid == other.dpid && @port_no == other.port_no
  end

  def hash
    name.hash
  end
end
