Feature: "Hello Trema!" example
  Scenario: Run
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id '0xabc' }
    """
    When I run `trema run ../../lib/hello_trema.rb -c trema.conf -d`
    And I run `sleep 1`
    Then a file named "log/HelloTrema.log" should exist
    And the file "log/HelloTrema.log" should contain "Hello 0xabc!"
