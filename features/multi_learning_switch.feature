Feature: "Multi Learning Switch" example
  Background:
    Given a file named "trema.conf" with:
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
    Given I run `trema run ../../lib/multi_learning_switch.rb -c trema.conf -p . -l . -s .` interactively
    And I wait for stdout to contain "MultiLearningSwitch started"
    And I run `sleep 10`
    When I run `trema send_packets --source host1 --dest host2 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     0 |     0 |     0 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     0 |
    When I run `trema send_packets --source host3 --dest host4 --n_pkts 3`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     0 |     3 |     0 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     3 |
    When I run `trema send_packets --source host4 --dest host1 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     0 |     3 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     0 |     3 |
    When I run `trema send_packets --source host2 --dest host3 --n_pkts 4`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     4 |     3 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     3 |
    When I run `trema send_packets --source host1 --dest host4 --n_pkts 1`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     3 |     4 |     3 |     2 |
     And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     4 |

  @sudo
  Scenario: Run as a daemon
    Given I successfully run `trema run ../../lib/multi_learning_switch.rb -c trema.conf -d -p . -l . -s .`
    And I wait until a file named "MultiLearningSwitch.pid" is created
    And I run `sleep 10`
    When I successfully run `trema send_packets --source host1 --dest host2 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     0 |     0 |     0 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     0 |
    When I successfully run `trema send_packets --source host3 --dest host4 --n_pkts 3`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     0 |     3 |     0 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     0 |     2 |     0 |     3 |
    When I successfully run `trema send_packets --source host4 --dest host1 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     0 |     3 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     0 |     3 |
    When I successfully run `trema send_packets --source host2 --dest host3 --n_pkts 4`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     4 |     3 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     3 |
    When I successfully run `trema send_packets --source host1 --dest host4 --n_pkts 1`
    Then the total number of tx packets should be:
      | host1 | host2 | host3 | host4 |
      |     3 |     4 |     3 |     2 |
     And the total number of rx packets should be:
      | host1 | host2 | host3 | host4 |
      |     2 |     2 |     4 |     4 |
