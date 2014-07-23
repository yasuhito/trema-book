# encoding: utf-8

# Forwarding database (FDB) entry.
class ForwardingEntry
  attr_reader :mac
  attr_reader :port_no
  attr_writer :age_max

  def initialize(mac, port_no, age_max)
    @mac = mac
    @port_no = port_no
    @age_max = age_max
    @last_update = Time.now
  end

  def update(port_no)
    @port_no = port_no
    @last_update = Time.now
  end

  def aged_out?
    Time.now - @last_update > @age_max
  end
end
