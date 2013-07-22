require "rubygems"
require "bindata"


class SystemDescriptionValue < BinData::Record
  endian :big

  stringz :system_description
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
