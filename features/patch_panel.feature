Feature: "Patch Panel" example
  Background:
    Given a file named "trema.conf" with:
    """
    vswitch('patch') { datapath_id 0xabc }

    vhost ('host1') { ip '192.168.0.1' }
    vhost ('host2') { ip '192.168.0.2' }

    link 'patch', 'host1'
    link 'patch', 'host2'
    """

  @sudo
  Scenario: Run
    Given a file named "patch_panel.conf" with:
    """
    1 2
    """
    And I run `trema -v run ../../lib/patch_panel.rb -c trema.conf -p . -l . -s .` interactively
    And I wait for stdout to contain "PatchPanel started (config = patch_panel.conf)"
    When I run `trema send_packets --source host1 --dest host2 --n_pkts 1`
    And I run `trema show_stats host1 --tx`
    And I run `trema show_stats host2 --rx`
    Then the output from "trema show_stats host1 --tx" should contain "192.168.0.2,1,192.168.0.1,1,1,50"
    And the output from "trema show_stats host2 --rx" should contain "192.168.0.2,1,192.168.0.1,1,1,50"

  @sudo
  Scenario: Run as a daemon
    Given a file named "patch_panel.conf" with:
    """
    1 2
    """
    And I successfully run `trema run ../../lib/patch_panel.rb -c trema.conf -d -p . -l . -s .`
    And I wait until a file named "PatchPanel.pid" is created
    And I run `sleep 5`
    When I run `trema send_packets --source host1 --dest host2 --n_pkts 1`
    And I run `trema show_stats host1 --tx`
    And I run `trema show_stats host2 --rx`
    Then the output from "trema show_stats host1 --tx" should contain "192.168.0.2,1,192.168.0.1,1,1,50"
    And the output from "trema show_stats host2 --rx" should contain "192.168.0.2,1,192.168.0.1,1,1,50"

  @sudo
  Scenario: Run with an argument
    Given a file named "my.conf" with:
    """
    1 2
    """
    And I successfully run `trema run ../../lib/patch_panel.rb -c trema.conf -d -p . -l . -s . -- my.conf`
    And a file named "PatchPanel.log" should exist
    And the file "PatchPanel.log" should contain "PatchPanel started (config = my.conf)"

  @sudo
  Scenario: Run with an argument (nonexistent file).
    When I run `trema run ../../lib/patch_panel.rb -c trema.conf -p . -l . -s . -- NONEXISTENT.conf`
    Then the exit status should not be 0
    Then the output should contain "error: No such file or directory - NONEXISTENT.conf"

  @sudo
  Scenario: Invalid configuration file
    Given a file named "patch_panel.conf" with:
    """
    INVALID CONFIGURATION
    """
    And I run `trema run ../../lib/patch_panel.rb -c trema.conf -p . -l . -s .`
    Then the output should contain "error: Invalid format: 'INVALID CONFIGURATION'"
    And the output should contain "RuntimeError"
