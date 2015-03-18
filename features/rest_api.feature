Feature: REST API
  Background:
    Given I send and accept JSON
    And I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And I successfully run `trema run ../../lib/routing_switch.rb -d -- --slicing`

  @rest_api
  Scenario: POST /slices
    When I send a POST request for "/slices" with body "{name: 'foo'}"
    Then the response should be "201"
    When I successfully run `slice list`
    Then the output should contain exactly "foo\n"

  @rest_api
  Scenario: DELETE /slices
    Given I successfully run `slice add foo`
    When I send a DELETE request for "/slices" with body "{name: 'foo'}"
    Then the response should be "200"
    When I successfully run `slice list`
    Then the output should contain exactly ""

  @rest_api
  Scenario: GET /slices (no slices)
    When I send a GET request for "/slices"
    Then the response should be "200"
    And the JSON response should be:
      """
      []
      """

  @rest_api
  Scenario: GET /slices (one slice)
    Given I successfully run `slice add foo`
    When I send a GET request for "/slices"
    Then the response should be "200"
    And the JSON response should be:
      """
      [{"name": "foo"}]
      """

  @rest_api
  Scenario: GET /slices (three slices)
    Given I successfully run `slice add foo`
    And I successfully run `slice add bar`
    And I successfully run `slice add baz`
    When I send a GET request for "/slices"
    Then the response should be "200"
    And the JSON response should be:
      """
      [{"name": "foo"}, {"name": "bar"}, {"name": "baz"}]
      """

  @rest_api
  Scenario: GET /slices/:slice_id
    Given I successfully run `slice add foo`
    When I send a GET request for "/slices/foo"
    Then the response should be "200"
    And the JSON response should be:
      """
      {"name": "foo"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id (404)
    Given I send a GET request for "/slices/bar"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Slice named 'bar' not found"}
      """

  @rest_api
  Scenario: POST /slices/:slice_id/ports
    When I send a POST request for "/slices" with body "{name: 'foo'}"
    And I send a POST request for "/slices/foo/ports" with body "{dpid: 1, port_no: 1}"
    Then the response should be "201"
    When I successfully run `slice list`
    Then the output should contain exactly:
      """
      foo
        0x1:1

      """

  @rest_api
  Scenario: DELETE /slices/:slice_id/ports
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    And I successfully run `slice delete_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a DELETE request for "/slices/foo/ports" with body "{dpid: 1, port_no: 1}"
    Then the response should be "200"
    When I successfully run `slice list`
    Then the output should contain exactly:
      """
      foo

      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports (no ports)
    Given I successfully run `slice add foo`
    When I send a GET request for "/slices/foo/ports"
    Then the response should be "200"
    And the JSON response should be:
      """
      []
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports (one port)
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a GET request for "/slices/foo/ports"
    Then the response should be "200"
    And the JSON response should be:
      """
      [{"name": "0x1:1", "dpid": 1, "port_no": 1}]
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports (three ports)
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:02 --port 0x2:1 --slice foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:03 --port 0x3:1 --slice foo`
    When I send a GET request for "/slices/foo/ports"
    Then the response should be "200"
    And the JSON response should be:
      """
      [{"name": "0x1:1", "dpid": 1, "port_no": 1}, {"name": "0x2:1", "dpid": 2, "port_no": 1}, {"name": "0x3:1", "dpid": 3, "port_no": 1}]
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports (404)
    When I send a GET request for "/slices/bar/ports"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Slice named 'bar' not found"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a GET request for "/slices/foo/ports/0x1:1"
    Then the response should be "200"
    And the JSON response should be:
      """
      {"name": "0x1:1", "dpid": 1, "port_no": 1}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id (:slice_id 404)
    When I send a GET request for "/slices/bar/ports/0x1:1"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Slice named 'bar' not found"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id (:port_id 404)
    Given I successfully run `slice add foo`
    When I send a GET request for "/slices/foo/ports/0x1:1"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Port named '0x1:1' not found"}
      """

  @rest_api
  Scenario: POST /slices/:slice_id/ports/:port_id/mac_addresses
    When I send a POST request for "/slices" with body "{name: 'foo'}"
    And I send a POST request for "/slices/foo/ports/0x1:1/mac_addresses" with body "{name: '00:00:00:00:00:01'}"
    Then the response should be "201"
    When I successfully run `slice list`
    Then the output should contain exactly:
      """
      foo
        0x1:1
          00:00:00:00:00:01

      """

  @rest_api
  Scenario: DELETE /slices/:slice_id/ports/:port_id/mac_addresses
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a DELETE request for "/slices/foo/ports/0x1:1/mac_addresses" with body "{name: '00:00:00:00:00:01'}"
    Then the response should be "200"
    When I successfully run `slice list`
    Then the output should contain exactly:
      """
      foo
        0x1:1

      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses (one MAC address)
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a GET request for "/slices/foo/ports/0x1:1/mac_addresses"
    Then the response should be "200"
    And the JSON response should be:
      """
      [{"name": "00:00:00:00:00:01"}]
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses (three MAC addresses)
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:02 --port 0x1:1 --slice foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:03 --port 0x1:1 --slice foo`
    When I send a GET request for "/slices/foo/ports/0x1:1/mac_addresses"
    Then the response should be "200"
    And the JSON response should be:
      """
      [{"name": "00:00:00:00:00:01"}, {"name": "00:00:00:00:00:02"}, {"name": "00:00:00:00:00:03"}]
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses (:slice_id 404)
    When I send a GET request for "/slices/bar/ports/0x1:1/mac_addresses"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Slice named 'bar' not found"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a GET request for "/slices/foo/ports/0x1:1/mac_addresses/00:00:00:00:00:01"
    Then the response should be "200"
    And the JSON response should be:
      """
      {"name": "00:00:00:00:00:01"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id (:slice_id 404)
    When I send a GET request for "/slices/bar/ports/0x1:1/mac_addresses/00:00:00:00:00:01"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Slice named 'bar' not found"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id (:port_id 404)
    Given I successfully run `slice add foo`
    When I send a GET request for "/slices/foo/ports/0x1:1/mac_addresses/00:00:00:00:00:01"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "Port named '0x1:1' not found"}
      """

  @rest_api
  Scenario: GET /slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id (:mac_address_id 404)
    Given I successfully run `slice add foo`
    And I successfully run `slice add_host --mac 00:00:00:00:00:01 --port 0x1:1 --slice foo`
    When I send a GET request for "/slices/foo/ports/0x1:1/mac_addresses/00:00:00:00:00:02"
    Then the response should be "404"
    And the JSON response should be:
      """
      {"error": "MAC address '00:00:00:00:00:02' not found"}
      """
