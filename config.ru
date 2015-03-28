$LOAD_PATH.unshift File.join(__dir__, 'lib')

require 'sliceable_switch/rest_api'

run SliceableSwitch::RestApi
