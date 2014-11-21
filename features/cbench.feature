Feature: cbench command
  Scenario: "Cbench is not yet compiled" error
    Given I run `rake clean`
    When I run `cbench --help`
    Then the output should contain:
    """
    Cbench is not yet compiled. Run `bundle exec rake`.
    """
