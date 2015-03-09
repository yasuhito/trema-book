@announce
Feature: routing switch
  @sudo
  Scenario: Create flow entries for shortest path
    Given a file named "trema.conf" with:
    """
    vswitch('switch1') { datapath_id 0x1 }
    vswitch('switch2') { datapath_id 0x2 }
    vswitch('switch3') { datapath_id 0x3 }
    vswitch('switch4') { datapath_id 0x4 }

    vhost('host1') { ip '192.168.0.1' }
    vhost('host2') { ip '192.168.0.2' }
    vhost('host3') { ip '192.168.0.3' }
    vhost('host4') { ip '192.168.0.4' }

    link 'switch1', 'host1'
    link 'switch2', 'host2'
    link 'switch3', 'host3'
    link 'switch4', 'host4'
    link 'switch1', 'switch2'
    link 'switch1', 'switch3'
    link 'switch1', 'switch4'
    link 'switch2', 'switch3'
    link 'switch2', 'switch4'
    link 'switch3', 'switch4'
    """
    When I run `trema run ../../lib/routing_switch.rb -c trema.conf -d -P . -L . -S .`
    And I run `sleep 8`
    And I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema show_stats host1 -S .`
    And I run `trema show_stats host2 -S .`
    And I run `trema show_stats host3 -S .`
    And I run `trema show_stats host4 -S .`
    And I run `trema dump_flows switch1 -S .`
    And I run `trema dump_flows switch2 -S .`
    And I run `trema dump_flows switch3 -S .`
    And I run `trema dump_flows switch4 -S .`
    Then the stdout from "trema show_stats host1 -S ." should contain:
    """
    192.168.0.1 -> 192.168.0.2 = 1 packet
    """
    Then the stdout from "trema show_stats host1 -S ." should contain:
    """
    192.168.0.2 -> 192.168.0.1 = 1 packet
    """
    And the stdout from "trema show_stats host2 -S ." should contain:
    """
    192.168.0.2 -> 192.168.0.1 = 1 packet
    """
    And the stdout from "trema show_stats host3 -S ." should not contain "Packets received"
    And the stdout from "trema show_stats host4 -S ." should not contain "Packets received"
    And the stdout from "trema dump_flows switch1 -S ." should contain:
    """
    nw_src=192.168.0.2,nw_dst=192.168.0.1
    """
    And the stdout from "trema dump_flows switch2 -S ." should contain:
    """
    nw_src=192.168.0.2,nw_dst=192.168.0.1
    """
    And the stdout from "trema dump_flows switch3 -S ." should not contain "nw_src"
    And the stdout from "trema dump_flows switch4 -S ." should not contain "nw_src"
