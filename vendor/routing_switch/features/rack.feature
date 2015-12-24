Feature: rack
  @sudo @rack
  Scenario: rackup
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     | 
    And I trema run "./lib/routing_switch.rb" with args "-d -- --slicing"
    And I successfully run `bash -c 'rackup ../../config.ru -P rack.pid &'`
    And I run `sleep 3`
    When I run `curl -s http://localhost:9292/slices`
    Then the stdout from "curl -s http://localhost:9292/slices" should contain exactly "[]"
