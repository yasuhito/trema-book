Feature: Virtual slicing
  Background:
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

      vhost('host1') {
        ip '192.168.0.1'
        mac '11:11:11:11:11:11'
      }
      vhost('host2') {
        ip '192.168.0.2'
        mac '22:22:22:22:22:22'
      }
      vhost('host3') {
        ip '192.168.0.3'
        mac '33:33:33:33:33:33'
      }
      vhost('host4') {
        ip '192.168.0.4'
        mac '44:44:44:44:44:44'
      }

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
    And I successfully run `trema run ../../lib/routing_switch.rb -c trema.conf -d -- --slicing`
    And I run `sleep 8`

  @sudo
  Scenario: no slices
    When I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        0 |
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |

  @sudo
  Scenario: add a slice then add two hosts to it
    When I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 22:22:22:22:22:22 --port 0x2:1 --slice foo`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        1 |
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |

  @sudo
  Scenario: delete a slice then paths also deleted
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 22:22:22:22:22:22 --port 0x2:1 --slice foo`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    And I run `trema send_packets --source host1 --dest host2`
    When I run `slice delete foo`
    Then the file "Path.log" should contain:
    """
    Deleting path: 22:22:22:22:22:22 -> 0x2:1 -> 0x2:2 -> 0x1:2 -> 0x1:1 -> 11:11:11:11:11:11
    """
    And the file "Path.log" should contain:
    """
    Deleting path: 11:11:11:11:11:11 -> 0x1:1 -> 0x1:2 -> 0x2:2 -> 0x2:1 -> 22:22:22:22:22:22
    """

  @sudo
  Scenario: delete a host then a path also deleted
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 22:22:22:22:22:22 --port 0x2:1 --slice foo`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    And I run `trema send_packets --source host1 --dest host2`
    When I run `slice delete_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo`
    Then the file "Path.log" should contain:
    """
    Deleting path: 22:22:22:22:22:22 -> 0x2:1 -> 0x2:2 -> 0x1:2 -> 0x1:1 -> 11:11:11:11:11:11
    """
    And the file "Path.log" should contain:
    """
    Deleting path: 11:11:11:11:11:11 -> 0x1:1 -> 0x1:2 -> 0x2:2 -> 0x2:1 -> 22:22:22:22:22:22
    """

  @sudo
  Scenario: delete a port then a path also deleted
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 22:22:22:22:22:22 --port 0x2:1 --slice foo`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    And I run `trema send_packets --source host1 --dest host2`
    When I run `slice delete_port --port 0x1:1 --slice foo`
    Then the file "Path.log" should contain:
    """
    Deleting path: 22:22:22:22:22:22 -> 0x2:1 -> 0x2:2 -> 0x1:2 -> 0x1:1 -> 11:11:11:11:11:11
    """
    And the file "Path.log" should contain:
    """
    Deleting path: 11:11:11:11:11:11 -> 0x1:1 -> 0x1:2 -> 0x2:2 -> 0x2:1 -> 22:22:22:22:22:22
    """

  @sudo
  Scenario: add wrong port number to a slice
    When I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 22:22:22:22:22:22 --port 0x2:2 --slice foo`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        0 |
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |
