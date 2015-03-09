Feature: slice command
  Scenario: slice add without slice name
    When I run `slice add`
    Then the stderr from "slice add" should contain "slice name is required."
    And the exit status should not be 0

  Scenario: slice add_host without MAC address
    When I run `slice add_host`
    Then the stderr from "slice add_host" should contain "MAC address is required."
    And the exit status should not be 0

  Scenario: slice add_host without --slice option
    When I run `slice add_host 11:22:33:44:55:66`
    Then the stderr from "slice add_host 11:22:33:44:55:66" should contain "--slice option is mandatory."
    And the exit status should not be 0
