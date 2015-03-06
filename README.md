routing_switch
==============
[![Build Status](http://img.shields.io/travis/trema/routing_switch/develop.svg?style=flat)][travis]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/routing_switch.svg?style=flat)][codeclimate]
[![Coverage Status](http://img.shields.io/codeclimate/coverage/github/trema/routing_switch.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/routing_switch.svg?style=flat)][gemnasium]

Layer 2 routing switch

[travis]: http://travis-ci.org/trema/routing_switch
[codeclimate]: https://codeclimate.com/github/trema/routing_switch
[gemnasium]: https://gemnasium.com/trema/routing_switch


Install
-------

```bash
$ git clone https://github.com/trema/routing_switch.git
$ cd routing_switch
$ bundle install
```


Play
----

```bash
$ bundle exec trema run lib/openflow_message_forwarder.rb -c trema.conf
Topology started (text mode).
Routing Switch started.
```
