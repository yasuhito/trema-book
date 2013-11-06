# -*- coding: utf-8 -*-
require 'trema'

module View
  #
  # Topology controller's CUI.
  #
  class Text
    include Trema::DefaultLogger

    def update(topology)
      topology.each_link do | each |
        info each.to_s
      end
      info 'topology updated'
    end
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
