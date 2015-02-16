Feature: "Learning Switch" example
  Background:
    And a file named "trema.conf" with:
    """
    vswitch('learning') { datapath_id 0xabc }

    vhost('host1') { ip '192.168.0.1' }
    vhost('host2') { ip '192.168.0.2' }

    link 'learning', 'host1'
    link 'learning', 'host2'
    """

  @sudo
  Scenario: Run as a daemon
    Given I run `trema run ../../lib/learning_switch.rb -c trema.conf -p . -l . -s .` interactively
    And I wait for stdout to contain "LearningSwitch started"
    And I run `sleep 5`
    When I run `trema send_packets --source host1 --dest host2 --n_pkts 1`
    And I run `trema send_packets --source host2 --dest host1 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 |
      |     1 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 |
      |     2 |     1 |

  @sudo
  Scenario: Run as a daemon
    Given I successfully run `trema run ../../lib/learning_switch.rb -c trema.conf -d -p . -l . -s .`
    And I wait until a file named "LearningSwitch.pid" is created
    And I run `sleep 5`
    When I successfully run `trema send_packets --source host1 --dest host2 --n_pkts 1`
    And I successfully run `trema send_packets --source host2 --dest host1 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 |
      |     1 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 |
      |     2 |     1 |
