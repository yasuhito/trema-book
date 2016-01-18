topology
========
[![Build Status](http://img.shields.io/travis/trema/topology/develop.svg?style=flat)][travis]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/topology.svg?style=flat)][codeclimate]
[![Coverage Status](http://img.shields.io/codeclimate/coverage/github/trema/topology.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/topology.svg?style=flat)][gemnasium]

Topology discovery controller in Trema.

[travis]: http://travis-ci.org/trema/topology
[codeclimate]: https://codeclimate.com/github/trema/topology
[gemnasium]: https://gemnasium.com/trema/topology


Prerequisites
-------------

* Ruby 2.0.0 or higher ([RVM][rvm]).
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).
* [Graphviz][graphviz] (`apt-get install graphviz`)

[rvm]: https://rvm.io/
[openvswitch]: https://openvswitch.org/
[graphviz]: http://www.graphviz.org/


Install
-------

```
$ git clone https://github.com/trema/topology.git
$ cd topology
$ bundle install --binstubs
```


Play
----

Run the controller `topology_controller.rb` with `trema run` by
passing a topology configuration file with `-c` option, then it
outputs the current topology information in ASCII format.

The triangle topology configuration with three switches:

```
$ ./bin/trema run ./lib/topology_controller.rb -c triangle.conf
```

The full mesh with 10 switches:

```
$ ./bin/trema run ./lib/topology_controller.rb -c fullmesh.conf
```

In another terminal, you can make changes to the current topology by
adding or deleting switches with `trema stop` and `trema start` commands.

```
$ ./bin/trema stop 0x1
$ ./bin/trema start 0x1
```

To turn switch ports on/off,

```
$ ./bin/trema port_down --switch 0x1 --port 1
$ ./bin/trema port_up --switch 0x1 --port 1
```

To view the current topology graphically,

```
$ ./bin/trema run ./lib/topology_controller.rb -c fullmesh.conf -- graphviz /tmp/topology.png
```

To change the LLDP destination MAC,

```
$ ./bin/trema run ./lib/topology_controller.rb -c fullmesh.conf -- --destination_mac 11:22:33:44:55:66
```
