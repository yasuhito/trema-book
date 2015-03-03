@announce
Feature: routing switch
  @sudo
  Scenario: Run
    Given a file named "trema.conf" with:
    """
    vswitch { datapath_id 0xabc }

    vhost('host1')
    vhost('host2')

    link '0xabc', 'host1'
    link '0xabc', 'host2'
    """
    When I run `trema run ../../lib/routing_switch.rb -c trema.conf -d -P . -L . -S .`
    And I run `sleep 3`
    Then a file named "RoutingSwitch.log" should exist
