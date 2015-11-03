Feature: Simple router
  Background:
    Given a file named "trema.conf" with:
      """
      vswitch('0x1') { dpid 0x1 }
      netns('host1') {
        ip '192.168.1.2'
        netmask '255.255.255.0'
        route net: '0.0.0.0', gateway: '192.168.1.1'
      }
      netns('host2') {
        ip '192.168.2.2'
        netmask '255.255.255.0'
        route net: '0.0.0.0', gateway: '192.168.2.1'
      }
      link '0x1', 'host1'
      link '0x1', 'host2'
      """
    And I trema run "lib/simple_router.rb" interactively with the configuration "trema.conf"
    And I run `sleep 10`
    When I run `bundle exec trema netns host1` interactively

  @sudo
  Scenario: ping router's interface
    When I type "ping 192.168.1.1 -c 3"
    Then the output from "bundle exec trema netns host1" should contain:
      """
      3 packets transmitted, 3 received, 0% packet loss
      """

  @sudo
  Scenario: ping another host
    When I type "ping 192.168.2.2 -c 3"
    Then the output from "bundle exec trema netns host1" should contain:
      """
      3 packets transmitted, 3 received, 0% packet loss
      """
