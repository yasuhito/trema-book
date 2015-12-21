Feature: "Patch Panel" example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "patch_panel.conf" with:
      """
      vswitch('patch_panel') { datapath_id 0xabc }

      vhost ('host1') { ip '192.168.0.1' }
      vhost ('host2') { ip '192.168.0.2' }
      vhost ('host3') { ip '192.168.0.3' }

      link 'patch_panel', 'host1'
      link 'patch_panel', 'host2'
      link 'patch_panel', 'host3'
      """
    And I trema run "lib/patch_panel.rb" with the configuration "patch_panel.conf"

  @sudo
  Scenario: create a patch (port #1 #2)
    When I successfully run `patch_panel create 0xabc 1 2`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host1 --dest host3`
    And I run `trema send_packets --source host2 --dest host1`
    And I run `trema send_packets --source host2 --dest host3`
    And I run `trema send_packets --source host3 --dest host1`
    And I run `trema send_packets --source host3 --dest host2`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        1 |
      | 192.168.0.3 |        0 |
    And the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        1 |
      | 192.168.0.3 |        0 |
    And the number of packets received by "host3" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |
      | 192.168.0.2 |        0 |

  @sudo
  Scenario: create a patch (port #1 and #2) then delete the patch
    Given I successfully run `patch_panel create 0xabc 1 2`
    When I successfully run `patch_panel delete 0xabc 1 2`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema send_packets --source host2 --dest host1`
    Then the number of packets received by "host1" should be:
      |      source | #packets |
      | 192.168.0.2 |        0 |
    And the number of packets received by "host2" should be:
      |      source | #packets |
      | 192.168.0.1 |        0 |
