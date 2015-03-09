routing_switch
==============
[![Build Status](http://img.shields.io/travis/trema/routing_switch/develop.svg?style=flat)][travis]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/routing_switch.svg?style=flat)][codeclimate]
[![Coverage Status](http://img.shields.io/codeclimate/coverage/github/trema/routing_switch.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/routing_switch.svg?style=flat)][gemnasium]

This is a layer 2 switch application with virtual slicing
function. The slicing function allows us to create multiple layer 2
domains. This is similar to MAC-based VLAN but there is no limitation
on VLAN ID space.

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

To run without virtual slicing, run
`lib/openflow_message_forwarder.rb` as follows:

```bash
$ bundle exec trema run lib/openflow_message_forwarder.rb -c trema.conf
Topology started (text mode).
Routing Switch started.
```

To run with virtual slicing support, run
`lib/openflow_message_forwarder.rb` with `-- --slicing` options as
follows:

```bash
$ bundle exec trema run lib/openflow_message_forwarder.rb -c trema.conf -- --slicing
Topology started (text mode).
Routing Switch started.
Sliceable switch started.
```
