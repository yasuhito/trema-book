Feature: Detect network topology
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "triangle.conf" with:
      """
      vswitch { dpid 0x1 }
      vswitch { dpid 0x2 }
      vswitch { dpid 0x3 }

      link '0x1', '0x2'
      link '0x1', '0x3'
      link '0x3', '0x2'
      """

  @sudo
  Scenario: Detect switch to switch links
    When I trema run "lib/topology_controller.rb" interactively with the configuration "triangle.conf"
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      0x1-0x2, 0x1-0x3, 0x2-0x3
      """

  @sudo
  Scenario: Run (args = graphviz)
    When I trema run "lib/topology_controller.rb" interactively with args "-c triangle.conf -- graphviz"
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Topology started (Graphviz mode, output = topology.png)
      """
    And a file named "topology.png" should exist

  @sudo
  Scenario: Run (args = graphviz foobar.png)
    When I trema run "lib/topology_controller.rb" interactively with args "-c triangle.conf -- graphviz foobar.png"
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Topology started (Graphviz mode, output = foobar.png)
      """
    And a file named "foobar.png" should exist

  @sudo
  Scenario: Kill a switch then the topology updated
    Given I trema run "lib/topology_controller.rb" interactively with the configuration "triangle.conf"
    When I run `trema stop 0x3`
    And sleep 2
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Switch 0x3 deleted: 0x1, 0x2
      """
    Then the stdout should contain:
      """
      Link 0x1-0x3 deleted
      """
    Then the stdout should contain:
      """
      Link 0x2-0x3 deleted
      """

  @sudo
  Scenario: Bring a port down then the topology updated
    Given I trema run "lib/topology_controller.rb" interactively with the configuration "triangle.conf"
    When I run `trema port_down --switch 0x3 --port 1`
    And sleep 2
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Port 0x3:1 deleted: 2
      """

  @sudo
  Scenario: Bring a port down and up then the topology updated
    Given I trema run "lib/topology_controller.rb" interactively with the configuration "triangle.conf"
    When I run `trema port_down --switch 0x3 --port 1`
    And I run `trema port_up --switch 0x3 --port 1`
    And sleep 2
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Port 0x3:1 added: 1, 2
      """
