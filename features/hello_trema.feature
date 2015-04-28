Feature: "Hello Trema!" example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
      """
      vswitch { datapath_id 0xabc }
      """

  @sudo
  Scenario: Run
    When I run `trema run ../../lib/hello_trema.rb -c trema.conf` interactively
    And I run `sleep 3`
    And I run `trema killall`
    Then the output should contain:
      """
      Trema started.
      Hello 0xabc!
      """
    And a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain:
      """
      Trema started.
      """
    And the file "HelloTrema.log" should contain:
      """
      Hello 0xabc!
      """

  @sudo
  Scenario: Run (OpenFlow 1.3)
    When I run `trema run ../../lib/hello_trema.rb --openflow13 -c trema.conf` interactively
    And I run `sleep 3`
    And I run `trema killall`
    Then the output should contain:
      """
      Trema started.
      Hello 0xabc!
      """
    And a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain:
      """
      Trema started.
      """
    And the file "HelloTrema.log" should contain:
      """
      Hello 0xabc!
      """

  @sudo
  Scenario: Run as a daemon
    When I run `trema run ../../lib/hello_trema.rb -c trema.conf -d`
    And I run `sleep 3`
    Then a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain:
      """
      Trema started.
      """
    And the file "HelloTrema.log" should contain:
      """
      Hello 0xabc!
      """

  @sudo
  Scenario: Run as a daemon (OpenFlow 1.3)
    When I run `trema run ../../lib/hello_trema.rb --openflow13 -c trema.conf -d`
    And I run `sleep 3`
    Then a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain:
      """
      Trema started.
      """
    And the file "HelloTrema.log" should contain:
      """
      Hello 0xabc!
      """
