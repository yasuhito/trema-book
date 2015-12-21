Feature: Multi-threaded cbench controller
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |

  @sudo
  Scenario: Run
    Given I trema run "lib/multi_threaded_cbench.rb" interactively
    And sleep 2
    When I run `cbench --port 6653 --switches 1 --loops 2 --delay 1000`
    And I run `trema killall --all`
    Then the stdout should contain:
      """
      RESULT: 1 switches 1 tests
      """

  @sudo
  Scenario: Run as a daemon
    Given I trema run "lib/multi_threaded_cbench.rb"
    When I run `cbench --port 6653 --switches 1 --loops 2 --delay 1000`
    Then the stdout should contain:
      """
      RESULT: 1 switches 1 tests
      """
