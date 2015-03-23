Feature: slice command
  Scenario: slice add without slice name
    When I run `slice add`
    Then the stderr should contain "slice name is required."
    And the exit status should not be 0

  Scenario: slice add_host without MAC address
    When I run `slice add_host`
    Then the stderr should contain "--mac option is mandatory."
    And the exit status should not be 0

  Scenario: slice add_host without --port option
    When I run `slice add_host --mac 11:22:33:44:55:66`
    Then the stderr should contain "--port option is mandatory."
    And the exit status should not be 0

  Scenario: slice add_host without --slice option
    When I run `slice add_host --mac 11:22:33:44:55:66 --port 0x1:1`
    Then the stderr should contain "--slice option is mandatory."
    And the exit status should not be 0
