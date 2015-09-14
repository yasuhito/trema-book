Feature: cbench controller
  @sudo
  Scenario: Run
    Given I run `trema run ../../lib/cbench.rb` interactively
    And I run `sleep 3`
    When I run `cbench --port 6653 --switches 1 --loops 2 --delay 1000`
    And I run `trema killall --all`
    Then the stdout should contain:
    """
    RESULT: 1 switches 1 tests
    """

  @sudo
  Scenario: Run as a daemon
    Given I successfully run `trema run ../../lib/cbench.rb -d`
    And I run `sleep 3`
    When I run `cbench --port 6653 --switches 1 --loops 2 --delay 1000`
    Then the stdout should contain:
    """
    RESULT: 1 switches 1 tests
    """
