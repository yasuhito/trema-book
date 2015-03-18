Feature: "Multi Learning Switch" example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
    """
    vswitch('lsw1') { datapath_id 0x1 }
    vswitch('lsw2') { datapath_id 0x2 }
    vswitch('lsw3') { datapath_id 0x3 }
    vswitch('lsw4') { datapath_id 0x4 }

    vhost('host1')
    vhost('host2')
    vhost('host3')
    vhost('host4')

    link 'lsw1', 'host1'
    link 'lsw2', 'host2'
    link 'lsw3', 'host3'
    link 'lsw4', 'host4'
    link 'lsw1', 'lsw2'
    link 'lsw2', 'lsw3'
    link 'lsw3', 'lsw4'
    """

  @sudo
  Scenario: Run
    Given I run `trema run ../../lib/multi_learning_switch.rb -c trema.conf` interactively
    And I run `sleep 10`
    When I run `trema send_packets --source host1 --dest host2 --npackets 2`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     0 |
    When I run `trema send_packets --source host3 --dest host4 --npackets 3`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     3 |
    When I run `trema send_packets --source host4 --dest host1 --npackets 2`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     0 |     3 |
    When I run `trema send_packets --source host2 --dest host3 --npackets 4`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     3 |
    When I run `trema send_packets --source host1 --dest host4 --npackets 1`
    And the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     4 |

  @sudo
  Scenario: Run as a daemon
    Given I successfully run `trema run ../../lib/multi_learning_switch.rb -c trema.conf -d`
    And I run `sleep 10`
    When I successfully run `trema send_packets --source host1 --dest host2 --npackets 2`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     0 |
    When I successfully run `trema send_packets --source host3 --dest host4 --npackets 3`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     3 |
    When I successfully run `trema send_packets --source host4 --dest host1 --npackets 2`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     0 |     3 |
    When I successfully run `trema send_packets --source host2 --dest host3 --npackets 4`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     3 |
    When I successfully run `trema send_packets --source host1 --dest host4 --npackets 1`
    Then the total number of received packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     4 |
