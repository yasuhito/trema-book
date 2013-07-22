require "rubygems"
require "bindata"


class SystemNameValue < BinData::Record
  endian :big

  stringz :system_name
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
