Feature: Detect network topology
  Background:
    Given a file named "triangle.conf" with:
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
    When I run `trema run ../../lib/topology_controller.rb -c triangle.conf` interactively
    And I run `sleep 5`
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      0x1-0x2, 0x1-0x3, 0x2-0x3
      """

  @sudo
  Scenario: Run (args = graphviz)
    When I run `trema run ../../lib/topology_controller.rb -c triangle.conf -- graphviz` interactively
    And I run `sleep 5`
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Topology started (Graphviz mode, output = topology.png)
      """
    And a file named "topology.png" should exist

  @sudo
  Scenario: Run (args = graphviz foobar.png)
    When I run `trema run ../../lib/topology_controller.rb -c triangle.conf -- graphviz foobar.png` interactively
    And I run `sleep 5`
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Topology started (Graphviz mode, output = foobar.png)
      """
    And a file named "foobar.png" should exist

  @sudo
  Scenario: Kill a switch then the topology updated
    Given I run `trema run ../../lib/topology_controller.rb -c triangle.conf` interactively
    And I run `sleep 5`
    When I run `trema stop 0x3`
    And I run `sleep 2`
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
    Given I run `trema run ../../lib/topology_controller.rb -c triangle.conf` interactively
    And I run `sleep 5`
    When I run `trema port_down --switch 0x3 --port 1`
    And I run `sleep 2`
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Port 0x3:1 deleted: 2
      """

  @sudo
  Scenario: Bring a port down and up then the topology updated
    Given I run `trema run ../../lib/topology_controller.rb -c triangle.conf` interactively
    And I run `sleep 5`
    When I run `trema port_down --switch 0x3 --port 1`
    And I run `trema port_up --switch 0x3 --port 1`
    And I run `sleep 2`
    And I run `trema killall TopologyController`
    Then the stdout should contain:
      """
      Port 0x3:1 added: 1, 2
      """
