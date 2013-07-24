require "bindata"


class MacAddress < BinData::Primitive
  array :octets, :type => :uint8, :initial_length => 6


  def set value
    self.octets = value.split( /:/ ).collect { | each | each.hex }
  end


  def get
    octets.inject( "" ) { | str, each | str + ( "%02x" % each ) }.hex
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
