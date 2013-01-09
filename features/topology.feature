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

      vhost "host1"
      vhost "host2"
      vhost "host3"

      link "0x1", "0x2"
      link "0x1", "0x3"
      link "0x1", "host1"
      link "0x3", "host2"
      link "0x1", "host3"
      """
    When I run `trema run ../../topology-controller.rb -c network.conf -d`
     And wait until "TopologyController" is up
     And *** sleep 10 ***
     And I run `trema killall`
    Then the log file "TopologyController.log" should contain "0x1 <-> 0x2"
     And the log file "TopologyController.log" should contain "0x1 <-> 0x3"
     And the log file "TopologyController.log" should contain "0x2 <-> 0x1"
     And the log file "TopologyController.log" should contain "0x3 <-> 0x1"
