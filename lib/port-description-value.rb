require "rubygems"
require "bindata"


class PortDescriptionValue < BinData::Record
  endian :big

  stringz :port_description
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
