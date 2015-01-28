Feature: "Patch Panel" example
  Background:
    Given a file named ".trema/config" with:
    """
    LOG_DIR: .
    PID_DIR: .
    SOCKET_DIR: .
    """
    And I successfully run `sudo -v`

  Scenario: Run
    Given a file named "patch_panel.conf" with:
    """
    1 2
    """
    And a file named "trema.conf" with:
    """
    vswitch('patch') {
      datapath_id '0xabc'
    }

    vhost ('host1') {
      ip '192.168.0.1'
    }

    vhost ('host2') {
      ip '192.168.0.2'
    }

    link 'patch', 'host1'
    link 'patch', 'host2'
    """
    And I successfully run `trema run ../../lib/patch_panel.rb -c trema.conf -d`
    And I run `sleep 5`
    When I successfully run `trema send_packets --source host1 --dest host2 --n_pkts 1`
    And I run `trema show_stats host1 --tx`
    And I run `trema show_stats host2 --rx`
    Then the output from "trema show_stats host1 --tx" should contain "192.168.0.2,1,192.168.0.1,1,1,50"
    And the output from "trema show_stats host2 --rx" should contain "192.168.0.2,1,192.168.0.1,1,1,50"
