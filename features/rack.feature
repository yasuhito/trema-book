Feature: rack
  @sudo @rack
  Scenario: rackup
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And I successfully run `trema run ../../lib/routing_switch.rb -d -- --slicing`
    And I run `sleep 3`
    And I successfully run `bash -c 'rackup ../../config.ru -P rack.pid &'`
    And I run `sleep 3`
    When I run `curl -s http://localhost:9292/slices`
    Then the stdout should contain "[]"
