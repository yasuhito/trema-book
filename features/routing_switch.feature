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
    When I run `trema run ../../lib/openflow_message_forwarder.rb -c trema.conf -d -P . -L . -S .`
    And I run `sleep 3`
    And I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema show_stats host1 -S .`
    And I run `trema show_stats host2 -S .`
    Then the stdout from "trema show_stats host1 -S ." should contain:
    """
    192.168.0.1 -> 192.168.0.2 = 1 packet
    """
    And the stdout from "trema show_stats host1 -S ." should contain:
    """
    192.168.0.2 -> 192.168.0.1 = 1 packet
    """
    And the stdout from "trema show_stats host2 -S ." should contain:
    """
    192.168.0.2 -> 192.168.0.1 = 1 packet
    """

  @sudo
  Scenario: Two switches
    Given a file named "trema.conf" with:
    """
    vswitch('switch1') { datapath_id 0x1 }
    vswitch('switch2') { datapath_id 0x2 }

    vhost('host1') { ip '192.168.0.1' }
    vhost('host2') { ip '192.168.0.2' }

    link 'switch1', 'host1'
    link 'switch2', 'host2'
    link 'switch1', 'switch2'
    """
    When I run `trema run ../../lib/openflow_message_forwarder.rb -c trema.conf -d -P . -L . -S .`
    And I run `sleep 8`
    And I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema show_stats host1 -S .`
    And I run `trema show_stats host2 -S .`
    Then the stdout from "trema show_stats host1 -S ." should contain:
    """
    192.168.0.1 -> 192.168.0.2 = 1 packet
    """
    Then the stdout from "trema show_stats host1 -S ." should contain:
    """
    192.168.0.2 -> 192.168.0.1 = 1 packet
    """
    And the stdout from "trema show_stats host2 -S ." should contain:
    """
    192.168.0.2 -> 192.168.0.1 = 1 packet
    """
