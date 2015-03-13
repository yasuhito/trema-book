require 'grape'
require 'trema'

class RoutingSwitch < Trema::Controller
  # REST API of RoutingSwitch
  class Api < Grape::API
    format :json
    prefix :api

    desc 'Lists slices'
    get :slices do
      []
    end
  end
end
