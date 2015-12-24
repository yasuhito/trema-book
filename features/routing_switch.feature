Feature: routing switch
  @sudo
  Scenario: Create flow entries for shortest path
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
      """
      vswitch('switch1') { datapath_id 0x1 }
      vswitch('switch2') { datapath_id 0x2 }
      vswitch('switch3') { datapath_id 0x3 }
      vswitch('switch4') { datapath_id 0x4 }

      vhost('host1') { ip '192.168.0.1' }
      vhost('host2') { ip '192.168.0.2' }
      vhost('host3') { ip '192.168.0.3' }
      vhost('host4') { ip '192.168.0.4' }

      link 'switch1', 'host1'
      link 'switch2', 'host2'
      link 'switch3', 'host3'
      link 'switch4', 'host4'
      link 'switch1', 'switch2'
      link 'switch1', 'switch3'
      link 'switch1', 'switch4'
      link 'switch2', 'switch3'
      link 'switch2', 'switch4'
      link 'switch3', 'switch4'
      """
    When I trema run "./lib/routing_switch.rb" with the configuration "trema.conf"
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        1 |
    And the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
    And the number of packets received by "host3" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |
      | 192.168.0.2 |        0 |
    And the number of packets received by "host4" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |
      | 192.168.0.2 |        0 |
    And the switch "switch1" has 1 flow entry
    And the switch "switch2" has 1 flow entry
    And the switch "switch3" has no flow entry
    And the switch "switch4" has no flow entry
