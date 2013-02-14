Feature: Detect network topology

  In order to detect network topology
  As a Trema user
  I want to run topology-controller.rb

  Background:
    Given I cd to "../.."

  Scenario: Detect switch to switch links
    When I run `trema run topology-controller.rb -c triangle.conf` interactively
     And I run `sleep 4`
     And I run `trema killall`
    Then the output should contain:
    """
    topology updated
    0x1 (port 1) <-> 0x3 (port 1)
    0x1 (port 2) <-> 0x2 (port 2)
    0x2 (port 1) <-> 0x3 (port 2)
    0x2 (port 2) <-> 0x1 (port 2)
    0x3 (port 1) <-> 0x1 (port 1)
    0x3 (port 2) <-> 0x2 (port 1)
    topology updated
    """

  Scenario: Kill a switch then the topology updated
    Given I run `trema run topology-controller.rb -c triangle.conf` interactively
     And I run `sleep 2`
    When I run `trema kill 0x3`
     And I run `sleep 2`
     And I run `trema killall`
    Then the output should contain:
    """
    topology updated
    0x1 (port 2) <-> 0x2 (port 2)
    0x2 (port 2) <-> 0x1 (port 2)
    topology updated
    """

  Scenario: Bring a port down then the topology updated
    Given I run `trema run topology-controller.rb -c triangle.conf` interactively
     And I run `sleep 2`
    When I run `trema port_down --switch 0x3 --port 1`
     And I run `sleep 2`
     And I run `trema killall`
    Then the output should contain:
    """
    topology updated
    0x1 (port 2) <-> 0x2 (port 2)
    0x2 (port 1) <-> 0x3 (port 2)
    0x2 (port 2) <-> 0x1 (port 2)
    0x3 (port 2) <-> 0x2 (port 1)
    topology updated
    """

  Scenario: Bring a port down and up then the topology updated
    Given I run `trema run topology-controller.rb -c triangle.conf` interactively
     And I run `sleep 2`
    When I run `trema port_down --switch 0x3 --port 1`
     And I run `trema port_up --switch 0x3 --port 1`
     And I run `sleep 2`
     And I run `trema killall`
    Then the output should contain:
    """
    topology updated
    0x1 (port 1) <-> 0x3 (port 1)
    0x1 (port 2) <-> 0x2 (port 2)
    0x2 (port 1) <-> 0x3 (port 2)
    0x2 (port 2) <-> 0x1 (port 2)
    0x3 (port 1) <-> 0x1 (port 1)
    0x3 (port 2) <-> 0x2 (port 1)
    topology updated
    """
