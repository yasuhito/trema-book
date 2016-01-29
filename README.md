Simple Router
=============

[![Build Status](http://img.shields.io/travis/trema/simple_router/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/simple_router/develop.svg?style=flat)][coveralls]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/simple_router.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/simple_router.svg?style=flat)][gemnasium]
[![Inline docs](http://inch-ci.org/github/trema/simple_router.png?branch=develop)][inch]

An OpenFlow controller that emulates layer 3 switch (router).

[travis]: http://travis-ci.org/trema/simple_router
[coveralls]: https://coveralls.io/r/trema/simple_router
[codeclimate]: https://codeclimate.com/github/trema/simple_router
[gemnasium]: https://gemnasium.com/trema/simple_router
[inch]: http://inch-ci.org/github/trema/simple_router


Prerequisites
-------------

* Ruby 2.0.0 or higher.
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).

[openvswitch]: https://openvswitch.org/


Install
-------

```
$ git clone https://github.com/trema/simple_router.git
$ cd simple_router
$ bundle install --binstubs
```


Play
----

The `lib/simple_router.rb` is an OpenFlow controller implementation
that emulates a layer 3 switch (router). Run this like so:

```
$ ./bin/trema run ./lib/simple_router.rb -c trema.conf
```

Then ping from host1 (192.168.1.2) to host2 (192.168.2.2):

```
$ ./bin/trema netns host1
$ ping 192.168.2.2
PING 192.168.2.2 (192.168.2.2) 56(84) bytes of data.
64 bytes from 192.168.2.2: icmp_seq=1 ttl=64 time=75.5 ms
64 bytes from 192.168.2.2: icmp_seq=2 ttl=64 time=82.3 ms
64 bytes from 192.168.2.2: icmp_seq=3 ttl=64 time=101 ms
64 bytes from 192.168.2.2: icmp_seq=4 ttl=64 time=83.3 ms
64 bytes from 192.168.2.2: icmp_seq=5 ttl=64 time=78.2 ms
64 bytes from 192.168.2.2: icmp_seq=6 ttl=64 time=76.4 ms
64 bytes from 192.168.2.2: icmp_seq=7 ttl=64 time=70.9 ms
^C
--- 192.168.2.2 ping statistics ---
7 packets transmitted, 7 received, 0% packet loss, time 6008ms
rtt min/avg/max/mdev = 70.995/81.159/101.180/9.050 ms
----
```

Enjoy!
