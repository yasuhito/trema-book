Feature: "Hello Trema!" example
  Background:
    Given a file named ".trema/config" with:
      """
      LOG_DIR: .
      PID_DIR: .
      SOCKET_DIR: .
      """
    And I successfully run `sudo -v`

  Scenario: Run
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id '0xabc' }
    """
    When I successfully run `trema run ../../lib/hello_trema.rb -c trema.conf -d`
    And I run `sleep 5`
    Then a file named "HelloTrema.log" should exist
    And the file "HelloTrema.log" should contain "Hello 0xabc!"
