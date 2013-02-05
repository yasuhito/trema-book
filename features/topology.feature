Feature: Detect network topology

  In order to detect network topology
  As a Trema user
  I want to run topology-controller.rb

  @slow_process
  Scenario: Detect switch <-> switch link
    Given a file named "network.conf" with:
      """
      vswitch { dpid "0x1" }
      vswitch { dpid "0x2" }
      vswitch { dpid "0x3" }

      link "0x1", "0x2"
      link "0x1", "0x3"
      link "0x3", "0x2"
      """
    When I run `trema run ../../topology-controller.rb -c network.conf -d`
     And wait until "TopologyController" is up
     And *** sleep 10 ***
     And I run `trema killall`
    Then the log file "TopologyController.log" should contain "0x1 (port 2) <-> 0x2 (port 2)"
     And the log file "TopologyController.log" should contain "0x1 (port 1) <-> 0x3 (port 1)"
     And the log file "TopologyController.log" should contain "0x2 (port 2) <-> 0x1 (port 2)"
     And the log file "TopologyController.log" should contain "0x2 (port 1) <-> 0x3 (port 2)"
     And the log file "TopologyController.log" should contain "0x3 (port 1) <-> 0x1 (port 1)"
     And the log file "TopologyController.log" should contain "0x3 (port 2) <-> 0x2 (port 1)"
