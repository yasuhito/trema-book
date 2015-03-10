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

    vhost('host1') {
      ip '192.168.0.1'
      mac '00:00:00:00:00:01'
    }
    vhost('host2') {
      ip '192.168.0.2'
      mac '00:00:00:00:00:02'
    }
    vhost('host3') {
      ip '192.168.0.3'
      mac '00:00:00:00:00:03'
    }
    vhost('host4') {
      ip '192.168.0.4'
      mac '00:00:00:00:00:04'
    }

    link 'switch1', 'host1'
    link 'switch2', 'host2'
    link 'switch3', 'host3'
    link 'switch4', 'host4'
    link 'switch1', 'switch2'
    link 'switch1', 'switch3'
    link 'switch2', 'switch4'
    link 'switch3', 'switch4'
    """
    When I run `trema -v run ../../lib/routing_switch.rb -c trema.conf -d -P . -L . -S .`
    And I run `sleep 8`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema delete_link switch1 switch2 -S .`
    And I run `sleep 3`
    And I run `trema dump_flows switch1 -S .`
    And I run `trema dump_flows switch2 -S .`
    And I run `trema dump_flows switch3 -S .`
    And I run `trema dump_flows switch4 -S .`
