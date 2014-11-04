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
