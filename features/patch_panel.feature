Feature: "Patch Panel" example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
      """
      vswitch('patch') { datapath_id 0xabc }

      vhost ('host1') { ip '192.168.0.1' }
      vhost ('host2') { ip '192.168.0.2' }

      link 'patch', 'host1'
      link 'patch', 'host2'
      """

  @sudo @announce
  Scenario: Run
    Given a file named "patch_panel.conf" with:
      """
      1 2
      """
    When I run `trema -v run ../../lib/patch_panel.rb -c trema.conf` interactively
    And I run `sleep 8`
    And I run `trema send_packets --source host1 --dest host2`
    And I run `trema show_stats host1`
    And I run `trema show_stats host2`
    And I run `trema killall`
    Then the stdout from "trema show_stats host1" should contain "192.168.0.2,1,192.168.0.1,1,1,50"
    And the stdout from "trema show_stats host2" should contain "192.168.0.2,1,192.168.0.1,1,1,50"

  @sudo
  Scenario: Run as a daemon
    Given a file named "patch_panel.conf" with:
      """
      1 2
      """
    And I successfully run `trema run ../../lib/patch_panel.rb -c trema.conf -d`
    And I run `sleep 8`
    When I run `trema send_packets --source host1 --dest host2`
    And I run `trema show_stats host1`
    And I run `trema show_stats host2`
    Then the stdout from "trema show_stats host1" should contain "192.168.0.2,1,192.168.0.1,1,1,50"
    And the stdout from "trema show_stats host2" should contain "192.168.0.2,1,192.168.0.1,1,1,50"

  @sudo
  Scenario: Default configuration file = patch_panel.conf
    Given a file named "patch_panel.conf" with:
      """
      1 2
      """
    And I successfully run `trema run ../../lib/patch_panel.rb -c trema.conf -d`
    And a file named "PatchPanel.log" should exist
    And the file "PatchPanel.log" should contain "PatchPanel started (config = patch_panel.conf)"

  @sudo
  Scenario: Configuration file = my.conf
    Given a file named "my.conf" with:
      """
      1 2
      """
    And I successfully run `trema run ../../lib/patch_panel.rb -c trema.conf -d -- my.conf`
    And a file named "PatchPanel.log" should exist
    And the file "PatchPanel.log" should contain "PatchPanel started (config = my.conf)"

  @sudo
  Scenario: Configuration file does not exist
    When I run `trema run ../../lib/patch_panel.rb -c trema.conf -- NONEXISTENT.conf`
    Then the exit status should not be 0
    Then the stderr should contain "error: No such file or directory - NONEXISTENT.conf"

  @sudo
  Scenario: Invalid configuration file
    Given a file named "patch_panel.conf" with:
      """
      INVALID CONFIGURATION
      """
    And I run `trema run ../../lib/patch_panel.rb -c trema.conf`
    Then the exit status should not be 0
    Then the stderr should contain "error: Invalid format: 'INVALID CONFIGURATION'"
    And the stderr should contain "RuntimeError"
