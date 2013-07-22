require "rubygems"
require "bindata"


class ManagementAddressValue < BinData::Record
  endian :big

  stringz :management_address
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
