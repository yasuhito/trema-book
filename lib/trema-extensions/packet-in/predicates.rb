module TremaExtensions
  module PacketIn
    def lldp?
      eth_type == 0x88CC
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
