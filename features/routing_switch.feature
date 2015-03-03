@announce
Feature: routing switch
  @sudo
  Scenario: One switch
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id 0xabc }

    vhost('host1') { ip '192.168.0.1' }
    vhost('host2') { ip '192.168.0.2' }

    link '0xabc', 'host1'
    link '0xabc', 'host2'
    """
    When I run `trema run ../../lib/routing_switch.rb -c trema.conf -d -P . -L . -S .`
    And I run `sleep 3`
    Then a file named "RoutingSwitch.log" should exist
    When I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema show_stats host1 -S .`
    And I run `trema show_stats host2 -S .`
    Then the stdout from "trema show_stats host1 -S ." should contain:
    """
    Packets sent:
      192.168.0.1 -> 192.168.0.2 = 1 packet
    Packets received:
      192.168.0.2 -> 192.168.0.1 = 1 packet
    """
    And the stdout from "trema show_stats host2 -S ." should contain:
    """
    Packets sent:
      192.168.0.2 -> 192.168.0.1 = 1 packet
    Packets received:
      192.168.0.1 -> 192.168.0.2 = 1 packet
    """
