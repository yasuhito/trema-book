Learning Switch
===============

[![Build Status](http://img.shields.io/travis/trema/learning_switch/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/learning_switch/develop.svg?style=flat)][coveralls]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/learning_switch.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/learning_switch.svg?style=flat)][gemnasium]
[![Inline docs](http://inch-ci.org/github/trema/learning_switch.png?branch=develop)][inch]

An OpenFlow controller that emulates layer 2 switches.

[travis]: http://travis-ci.org/trema/learning_switch
[coveralls]: https://coveralls.io/r/trema/learning_switch
[codeclimate]: https://codeclimate.com/github/trema/learning_switch
[gemnasium]: https://gemnasium.com/trema/learning_switch
[inch]: http://inch-ci.org/github/trema/learning_switch


Prerequisites
-------------

* Ruby 2.0.0 or higher.
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).

[openvswitch]: https://openvswitch.org/


Install
-------

```
$ git clone https://github.com/trema/learning_switch.git
$ cd learning_switch
$ bundle install
```


Play
----

The `lib/learning_switch.rb` is an OpenFlow controller implementation
that emulates a layer 2 switch. Run this like so:

```
$ ./bin/trema run ./lib/learning_switch.rb -c trema.conf
```

Then send some packets from host1 to host2, and show received packet
stats of host2:

```
$ ./bin/trema send_packets --source host1 --dest host2 --npackets 10
$ ./bin/trema show_stats host2
Packets received:
 192.168.0.1 -> 192.168.0.2 = 10 packets
```

The `lib/multi_learning_switch.rb` is an extension to
`lib/learning_switch.rb`. It can emulate multiple OpenFlow switches.

```
$ ./bin/trema run ./lib/multi_learning_switch.rb -c trema.multi.conf
```

Enjoy!
