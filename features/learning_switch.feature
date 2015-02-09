Feature: "Learning Switch" example
  Background:
    Given a file named ".trema/config" with:
    """
    LOG_DIR: .
    PID_DIR: .
    SOCKET_DIR: .
    """
    And a file named "trema.conf" with:
    """
    vswitch('learning') { datapath_id '0xabc' }

    vhost('host1') { ip '192.168.0.1' }
    vhost('host2') { ip '192.168.0.2' }

    link 'learning', 'host1'
    link 'learning', 'host2'
    """

  Scenario: Run
    Given I successfully run `trema run ../../lib/learning_switch.rb -c trema.conf -d`
    And I run `sleep 5`
    When I successfully run `trema send_packets --source host1 --dest host2 --n_pkts 1`
    And I successfully run `trema send_packets --source host2 --dest host1 --n_pkts 2`
    Then the total number of tx packets should be:
      | host1 | host2 |
      |     1 |     2 |
    And the total number of rx packets should be:
      | host1 | host2 |
      |     2 |     1 |
