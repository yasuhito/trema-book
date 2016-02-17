switch_monitor
==============

[![Build Status](http://img.shields.io/travis/trema/switch_monitor/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/switch_monitor/develop.svg?style=flat)][coveralls]
[![Dependency Status](http://img.shields.io/gemnasium/trema/switch_monitor.svg?style=flat)][gemnasium]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/switch_monitor.svg?style=flat)][codeclimate]
[![Inline docs](http://inch-ci.org/github/trema/switch_monitor.png?branch=develop)][inch]

OpenFlow switch liveness monitor.

[travis]: http://travis-ci.org/trema/switch_monitor
[coveralls]: https://coveralls.io/r/trema/switch_monitor
[codeclimate]: https://codeclimate.com/github/trema/switch_monitor
[gemnasium]: https://gemnasium.com/trema/switch_monitor
[inch]: http://inch-ci.org/github/trema/switch_monitor


Prerequisites
-------------

* Ruby 2.0.0 or higher ([RVM][rvm]).
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).

[rvm]: https://rvm.io/
[openvswitch]: https://openvswitch.org/


Install
-------

```
$ git clone https://github.com/trema/switch_monitor.git
$ cd switch_monitor
$ bundle install --binstubs
```


Play
----

Run this controller:

```
$ ./bin/trema run ./lib/switch_monitor.rb -c trema.conf
0x3 is up (All = 0x3)
0x2 is up (All = 0x2, 0x3)
0x1 is up (All = 0x1, 0x2, 0x3)
...
```

On another terminal, stop and restart running switches with `trema
stop` and `trema start` command, and switch_monitor.rb outputs the
current switch status.

```
(On another terminal)
$ ./bin/trema stop 0x2
$ ./bin/trema start 0x2
```

```
$ ./bin/trema run ./lib/switch_monitor.rb -c trema.conf
0x3 is up (All = 0x3)
0x2 is up (All = 0x2, 0x3)
0x1 is up (All = 0x1, 0x2, 0x3)
0x2 is down (All = 0x1, 0x3)
0x2 is up (All = 0x1, 0x2, 0x3)
```

Enjoy!
