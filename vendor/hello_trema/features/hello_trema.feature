@sudo
Feature: "Hello Trema!" example
  Background:
    Given I set the environment variables to:
      | variable         | value |
      | TREMA_LOG_DIR    | .     |
      | TREMA_PID_DIR    | .     |
      | TREMA_SOCKET_DIR | .     |
    And a file named "trema.conf" with:
      """
      vswitch { datapath_id 0xabc }
      """

  @open_flow10
  Scenario: Run
    Given I use OpenFlow 1.0
    When I trema run "lib/hello_trema.rb" interactively with the configuration "trema.conf"
    And I trema killall "HelloTrema"
    Then the output should contain:
      """
      Trema started.
      Hello 0xabc!
      """
    And the log file "HelloTrema.log" should contain following messages:
      | messages       |
      | Trema started. |
      | Hello 0xabc!   |

  @open_flow10
  Scenario: Run as a daemon
    Given I use OpenFlow 1.0
    When I trema run "lib/hello_trema.rb" with the configuration "trema.conf"
    Then the log file "HelloTrema.log" should contain following messages:
      | messages       |
      | Trema started. |
      | Hello 0xabc!   |

  @open_flow13
  Scenario: Run (OpenFlow 1.3)
    Given I use OpenFlow 1.3
    When I trema run "lib/hello_trema.rb" interactively with the configuration "trema.conf"
    And I trema killall "HelloTrema"
    Then the output should contain:
      """
      Trema started.
      Hello 0xabc!
      """
    And the log file "HelloTrema.log" should contain following messages:
      | messages       |
      | Trema started. |
      | Hello 0xabc!   |

  @open_flow13
  Scenario: Run as a daemon (OpenFlow 1.3)
    Given I use OpenFlow 1.3
    When I trema run "lib/hello_trema.rb" with the configuration "trema.conf"
    Then the log file "HelloTrema.log" should contain following messages:
      | messages       |
      | Trema started. |
      | Hello 0xabc!   |
