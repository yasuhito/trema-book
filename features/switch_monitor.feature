Feature: Switch Monitor example
  Background:
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id 0x1 }
    vswitch { datapath_id 0x2 }
    vswitch { datapath_id 0x3 }
    """

  @sudo
  Scenario: Run
    When I run `trema run ../../lib/switch_monitor.rb -c trema.conf -p . -l . -s .` interactively
    Then I wait for stdout to contain "SwitchMonitor started"
    And I wait for stdout to contain "0x1 is up"
    And I wait for stdout to contain "0x2 is up"
    And I wait for stdout to contain "0x3 is up"
    When I run `trema kill 0x2`
    And I wait for stdout to contain "0x2 is down (all = 0x1, 0x3)"
    When I run `trema up 0x2`
    And I wait for stdout to contain "0x2 is up (all = 0x1, 0x2, 0x3)"

  @sudo
  Scenario: Run as a daemon
    When I successfully run `trema run ../../lib/switch_monitor.rb -c trema.conf -d -p . -l . -s .`
    And I run `sleep 10`
    Then a file named "SwitchMonitor.log" should exist
    And the file "SwitchMonitor.log" should contain "0x1 is up"
    And the file "SwitchMonitor.log" should contain "0x2 is up"
    And the file "SwitchMonitor.log" should contain "0x3 is up"
    And the file "SwitchMonitor.log" should contain "all = 0x1, 0x2, 0x3"
    When I successfully run `trema kill 0x2`
    Then the file "SwitchMonitor.log" should contain "0x2 is down (all = 0x1, 0x3)"
    And a file named "open_vswitch.0x2.pid" should not exist
    When I successfully run `trema up 0x2`
    Then a file named "open_vswitch.0x2.pid" should exist
    And the file "SwitchMonitor.log" should contain "0x2 is up (all = 0x1, 0x2, 0x3)"
    
