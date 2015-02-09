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

Install
-------

```
$ git clone https://github.com/trema/switch_monitor.git
$ cd switch_monitor
$ bundle install
```


Play
----

Run this controller:

```
% bundle exec trema run ./lib/switch_monitor.rb -c trema.conf
All switches =
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
...
```

On another terminal, stop and restart running switches with `trema
kill` and `trema up` command, and switch_monitor.rb outputs the
current switch status.

```
(On another terminal)
% bundle exec trema kill 0x1
% bundle exec trema up 0x1
```

```
% bundle exec trema run ./lib/switch_monitor.rb -c trema.conf
All switches =
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
...
Switch 0x1 is DOWN
All switches = 0x2, 0x3
...
Switch 0x1 is UP
```

Enjoy!
