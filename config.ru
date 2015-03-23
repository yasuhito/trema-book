$LOAD_PATH.unshift File.join(__dir__, 'lib')

require 'routing_switch/rest_api'

run RoutingSwitch::RestApi
