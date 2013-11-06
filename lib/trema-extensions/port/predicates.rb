# -*- coding: utf-8 -*-
require 'trema'

module TremaExtensions
  #
  # Monkey patches for Trema::Port.
  #
  module Port
    def local?
      number == Trema::Controller::OFPP_LOCAL
    end
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
