@announce
Feature: Switch Monitor example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
      """
      vswitch { datapath_id 0x1 }
      vswitch { datapath_id 0x2 }
      vswitch { datapath_id 0x3 }
      """

  @sudo
  Scenario: Run
    When I run `trema run ../../lib/switch_monitor.rb -c trema.conf` interactively
    And I run `sleep 3`
    And I run `trema kill 0x2`
    And I run `trema up 0x2`
    And I run `trema killall`
    Then the stdout should contain "SwitchMonitor started"
    And the stdout should contain "0x1 is up"
    And the stdout should contain "0x2 is up"
    And the stdout should contain "0x3 is up"
    And the stdout should contain:
      """
      0x2 is down (all = 0x1, 0x3)
      0x2 is up (all = 0x1, 0x2, 0x3)
      """

  @sudo
  Scenario: Run as a daemon
    When I successfully run `trema run ../../lib/switch_monitor.rb -c trema.conf -d`
    And I run `sleep 3`
    Then a file named "SwitchMonitor.log" should exist
    And the file "SwitchMonitor.log" should contain "SwitchMonitor started"
    And the file "SwitchMonitor.log" should contain "0x1 is up"
    And the file "SwitchMonitor.log" should contain "0x2 is up"
    And the file "SwitchMonitor.log" should contain "0x3 is up"
    And the file "SwitchMonitor.log" should contain "all = 0x1, 0x2, 0x3"
    When I successfully run `trema kill 0x2`
    Then the file "SwitchMonitor.log" should contain "0x2 is down (all = 0x1, 0x3)"
    When I successfully run `trema up 0x2`
    And the file "SwitchMonitor.log" should contain "0x2 is up (all = 0x1, 0x2, 0x3)"
    
