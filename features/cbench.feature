Feature: cbench command
  Background:
    Given a file named ".trema/config" with:
    """
    LOG_DIR: .
    PID_DIR: .
    SOCKET_DIR: .
    """
    And I successfully run `rake`

  Scenario: Run
    Given I successfully run `sudo -v`    
    And I successfully run `trema run ../../lib/cbench_switch.rb -d`
    And I run `sleep 5`
    When I run `cbench --port 6633 --switches 1 --loops 2 --delay 1000`
    Then the output should contain:
    """
    RESULT: 1 switches 1 tests
    """

  Scenario: "Cbench is not yet compiled" error
    Given I run `rake clean`
    When I run `cbench --help`
    Then the output should contain:
    """
    Cbench is not yet compiled. Run `bundle exec rake`.
    """
