require "trema"


class TopologyView
  include Trema::Logger


  def update links
    links.each do | each |
      info each.to_s
    end
    info "topology updated"
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
