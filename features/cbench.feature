Feature: cbench command
  Background:
    Given a file named ".trema/config" with:
    """
    LOG_DIR: .
    PID_DIR: .
    SOCKET_DIR: .
    """
    And I successfully run `rake`

  @announce
  Scenario: Run
    Given I successfully run `sudo -v`    
    And I successfully run `trema run ../../lib/cbench_switch.rb -d`
    And I run `sleep 5`
    When I run `cbench --port 6653 --switches 1 --loops 10 --delay 1000`
    Then the output should contain:
    """
    cbench
    """

  Scenario: "Cbench is not yet compiled" error
    Given I run `rake clean`
    When I run `cbench --help`
    Then the output should contain:
    """
    Cbench is not yet compiled. Run `bundle exec rake`.
    """
