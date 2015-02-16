# A database that keeps pairs of a MAC address and a port number
class FDB
  # Forwarding database (FDB) entry.
  class Entry
    DEFAULT_AGE_MAX = 300

    attr_reader :mac
    attr_reader :port_no

    def initialize(mac, port_no, age_max = DEFAULT_AGE_MAX)
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
      @db[mac] = Entry.new(mac, port_no)
    end
  end

  def age
    @db.delete_if { |_mac, entry| entry.aged_out? }
  end
end
