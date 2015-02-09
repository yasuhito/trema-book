Learning Switch
===============

[![Build Status](http://img.shields.io/travis/trema/learning_switch/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/learning_switch/develop.svg?style=flat)][coveralls]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/learning_switch.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/learning_switch.svg?style=flat)][gemnasium]
[![Inline docs](http://inch-ci.org/github/trema/learning_switch.png?branch=develop)][inch]

An OpenFlow controller that controls a single OpenFlow switch and
emulates a layer 2 switch.

[travis]: http://travis-ci.org/trema/learning_switch
[coveralls]: https://coveralls.io/r/trema/learning_switch
[codeclimate]: https://codeclimate.com/github/trema/learning_switch
[gemnasium]: https://gemnasium.com/trema/learning_switch
[inch]: http://inch-ci.org/github/trema/learning_switch

Install
-------

```
$ git clone https://github.com/trema/learning_switch.git
$ cd learning_switch
$ bundle install
```


Play
----

Run this controller:

```
% bundle exec trema run lib/learning_switch.rb -c trema.conf
```

Send some packets from host1 to host2, and show received packet stats
of host2:

```
% bundle exec trema send_packets --source host1 --dest host2 --n_pkts 10
% bundle exec trema show_stats host2 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.2,1,192.168.0.1,1,10,500
```

Enjoy!
