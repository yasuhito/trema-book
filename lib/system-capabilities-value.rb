require "rubygems"
require "bindata"


class SystemCapabilitiesValue < BinData::Record
  endian :big

  uint16be :system_capabilities
  uint16be :enabled_capabilities
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
