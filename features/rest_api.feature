Feature: List slices
  Scenario: retrieve all slices as JSON
    Given I send and accept JSON
    When I send a GET request for "/api/slices"
    Then the response should be "200"
    And the JSON response should be an empty array
