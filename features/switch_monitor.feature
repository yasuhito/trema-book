Feature: Switch Monitor example
  Background:
    Given a file named ".trema/config" with:
    """
    LOG_DIR: .
    PID_DIR: .
    SOCKET_DIR: .
    """

  Scenario: Run
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id 0x1 }
    vswitch { datapath_id 0x2 }
    vswitch { datapath_id 0x3 }
    """
    When I successfully run `trema run ../../lib/switch_monitor.rb -c trema.conf -d`
    And I run `sleep 5`
    Then a file named "SwitchMonitor.log" should exist
    And the file "SwitchMonitor.log" should contain "Switch 0x1 is UP"
    And the file "SwitchMonitor.log" should contain "Switch 0x2 is UP"
    And the file "SwitchMonitor.log" should contain "Switch 0x3 is UP"
    When I successfully run `trema kill 0x2`
    Then the file "SwitchMonitor.log" should contain "Switch 0x2 is DOWN"
    When I successfully run `trema up 0x2`
    Then the file "SwitchMonitor.log" should contain "Switch 0x2 is UP"
