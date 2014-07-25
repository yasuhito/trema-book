Feature: "Hello Trema!" example

  The "Hello Trema!" example is one of the simplest OpenFlow
  controller implementation. The basic functionality of this
  controller is to establish a secure channel connection with an
  OpenFlow switch and output the "Hello [switch's dpid]!" message.

  This demonstrates a minimum template for Trema applications written
  in Ruby. Hence it's a good starting point to learn about Trema
  programming.

  Scenario: Run
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id '0xabc' }
    """
    When I run `trema run ../../hello_trema.rb -c trema.conf -d`
    And I run `sleep 1`
    Then a file named "log/HelloTrema.log" should exist
    And the file "log/HelloTrema.log" should contain "Hello 0xabc!"
