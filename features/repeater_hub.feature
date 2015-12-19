Feature: "Repeater Hub" example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
      """
      vswitch('repeater_hub') { datapath_id 0xabc }

      vhost('host1') {
        ip '192.168.0.1'
        promisc true
      }
      vhost('host2') {
        ip '192.168.0.2'
        promisc true
      }
      vhost('host3') {
        ip '192.168.0.3'
        promisc true
      }

      link 'repeater_hub', 'host1'
      link 'repeater_hub', 'host2'
      link 'repeater_hub', 'host3'
      """

  @sudo
  Scenario: Run
    Given I trema run "lib/repeater_hub.rb" interactively with the configuration "trema.conf"
    When I run `trema send_packets --source host1 --dest host2`
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
    And the number of packets received by "host3" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
  
  @sudo
  Scenario: Run as a daemon
    Given I trema run "lib/repeater_hub.rb" with the configuration "trema.conf"
    When I successfully run `trema send_packets --source host1 --dest host2`
    Then the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
    And the number of packets received by "host3" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
