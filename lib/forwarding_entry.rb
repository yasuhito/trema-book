# encoding: utf-8

# Forwarding database (FDB) entry.
class ForwardingEntry
  include DefaultLogger

  attr_reader :mac
  attr_reader :port_no
  attr_reader :dpid
  attr_writer :age_max

  def initialize(mac, port_no, age_max, dpid)
    @mac = mac
    @port_no = port_no
    @age_max = age_max
    @dpid = dpid
    @last_update = Time.now
    debug "New entry: MAC address = #{@mac}, #port = #{@port_no}"
  end

  def update(port_no)
    debug "Update: MAC address = #{@mac}, #port = (#{@port_no} => #{port_no})"
    @port_no = port_no
    @last_update = Time.now
  end

  def aged_out?
    aged_out = Time.now - @last_update > @age_max
    debug "Age out: MAC address = #{@mac}, #port = #{@port_no}" if aged_out
    aged_out
  end
end
