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
        mac '00:00:00:00:00:01'
      }
      vhost('host2') {
        ip '192.168.0.2'
        mac '00:00:00:00:00:02'
      }
      vhost('host3') {
        ip '192.168.0.3'
        mac '00:00:00:00:00:03'
      }
      vhost('host4') {
        ip '192.168.0.4'
        mac '00:00:00:00:00:04'
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
  Scenario: No slices
    When I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        0 |
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |

  @sudo
  Scenario: Add one slice then add two hosts to it
    When I run `slice add foo`
    And I run `slice add_host 00:00:00:00:00:01 --slice foo`
    And I run `slice add_host 00:00:00:00:00:02 --slice foo`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        1 |
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
