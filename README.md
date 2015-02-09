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
% bundle exec trema run lib/learning_switch.rb -c trema.conf
```

Then send some packets from host1 to host2, and show received packet
stats of host2:

```
% bundle exec trema send_packets --source host1 --dest host2 --n_pkts 10
% bundle exec trema show_stats host2 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.2,1,192.168.0.1,1,10,500
```

The `lib/multi_learning_switch.rb` is an extension to
`lib/learning_switch.rb`. It can emulate multiple OpenFlow switches.

```
% trema run ./lib/multi_learning_switch.rb -c trema.multi.conf
```


Enjoy!
