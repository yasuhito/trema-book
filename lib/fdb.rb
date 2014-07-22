# encoding: utf-8

require 'forwarding_entry'

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
