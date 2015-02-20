Feature: cbench command
  @sudo
  Scenario: Run
    Given I run `trema run ../../lib/cbench_switch.rb -p . -l . -s .` interactively
    And I run `sleep 3`
    When I run `cbench --port 6633 --switches 1 --loops 2 --delay 1000`
    And I run `trema killall`
    Then the stdout should contain:
    """
    RESULT: 1 switches 1 tests
    """

  @sudo
  Scenario: Run as a daemon
    Given I successfully run `trema run ../../lib/cbench_switch.rb -d -p . -l . -s .`
    And I run `sleep 3`
    When I run `cbench --port 6633 --switches 1 --loops 2 --delay 1000`
    Then the stdout should contain:
    """
    RESULT: 1 switches 1 tests
    """
