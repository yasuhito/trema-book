require 'aruba/cucumber'
require 'routing_switch/rest_api'

def app
  RoutingSwitch::RestApi
end
