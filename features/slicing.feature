@announce
Feature: Virtual slicing
  Background:
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
    link 'switch1', 'switch4'
    link 'switch2', 'switch3'
    link 'switch2', 'switch4'
    link 'switch3', 'switch4'
    """
    When I successfully run `trema run ../../lib/routing_switch.rb -c trema.conf -d -P . -L . -S . -- --slicing`
    And I run `sleep 8`

  @sudo
  Scenario: No slices
    And I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema show_stats host1 -S .`
    And I run `trema show_stats host2 -S .`
    Then the stdout from "trema show_stats host1 -S ." should not contain "192.168.0.2 -> 192.168.0.1 = 1 packet"

  @sudo
  Scenario: Add one slice then add two hosts to it
    And I run `slice add foo -S .`
    And I run `slice add_host 00:00:00:00:00:01 --slice foo -S .`
    And I run `slice add_host 00:00:00:00:00:02 --slice foo -S .`
    And I run `trema send_packets --source host1 --dest host2 -S .`
    And I run `trema send_packets --source host2 --dest host1 -S .`
    And I run `trema show_stats host1 -S .`
    And I run `trema show_stats host2 -S .`
    Then the stdout from "trema show_stats host1 -S ." should contain "192.168.0.2 -> 192.168.0.1 = 1 packet"
