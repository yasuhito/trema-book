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
