# encoding: utf-8

# Forwarding database (FDB)
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

# A database that keep pairs of MAC address and port number
class FDB
  DEFAULT_AGE_MAX = 300

  def initialize
    @db = {}
  end

  def port_no_of(mac)
    dest = @db[mac]
    if dest
      dest.port_no
    else
      nil
    end
  end

  def lookup(mac)
    dest = @db[mac]
    if dest
      [dest.dpid, dest.port_no]
    else
      nil
    end
  end

  def learn(mac, port_no, dpid = nil)
    entry = @db[mac]
    if entry
      entry.update port_no
    else
      new_entry = ForwardingEntry.new(mac, port_no, DEFAULT_AGE_MAX, dpid)
      @db[new_entry.mac] = new_entry
    end
  end

  def age
    @db.delete_if do |_mac, entry|
      entry.aged_out?
    end
  end
end
