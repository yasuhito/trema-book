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
    When I run `trema run ../../lib/hello_trema.rb -c trema.conf -- foo bar baz` interactively
    And I run `sleep 3`
    And I run `trema killall`
    Then the output should contain:
      """
      Trema started (args = ["foo", "bar", "baz"]).
      Hello 0xabc!
      """
    And a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain:
      """
      Trema started (args = ["foo", "bar", "baz"]).
      """
    And the file "HelloTrema.log" should contain:
      """
      Hello 0xabc!
      """

  @sudo
  Scenario: Run as a daemon
    When I run `trema run ../../lib/hello_trema.rb -c trema.conf -d -- foo bar baz`
    And I run `sleep 3`
    Then a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain:
      """
      Trema started (args = ["foo", "bar", "baz"]).
      """
    And the file "HelloTrema.log" should contain:
      """
      Hello 0xabc!
      """
