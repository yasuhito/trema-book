# encoding: utf-8

require 'forwarding_entry'

# A database that keeps pairs of a MAC address and a port number
class FDB
  DEFAULT_AGE_MAX = 300

  def initialize
    @db = {}
  end

  def lookup(mac)
    entry = @db[mac]
    entry && entry.port_no
  end

  def learn(mac, port_no)
    entry = @db[mac]
    if entry
      entry.update port_no
    else
      @db[mac] = ForwardingEntry.new(mac, port_no, DEFAULT_AGE_MAX)
    end
  end

  def age
    @db.delete_if do |_mac, entry|
      entry.aged_out?
    end
  end
end
