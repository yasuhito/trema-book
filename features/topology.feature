Feature: Detect network topology

  In order to detect network topology
  As a Trema user
  I want to run topology-controller.rb

  Scenario: Detect switch <-> switch link
    Given I cd to "../.."
    When I run `trema run topology-controller.rb -c network.conf` interactively
     And I wait for output to contain "topology updated"
     And I run `trema killall`
    Then the output should contain:
    """
    0x1 (port 1) <-> 0x3 (port 1)
    0x1 (port 2) <-> 0x2 (port 2)
    0x2 (port 1) <-> 0x3 (port 2)
    0x2 (port 2) <-> 0x1 (port 2)
    0x3 (port 1) <-> 0x1 (port 1)
    0x3 (port 2) <-> 0x2 (port 1)
    """
