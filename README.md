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

```bash
git clone https://github.com/trema/routing_switch.git
cd routing_switch
bundle install
```


Play
----

To run without virtual slicing, run `lib/routing_switch.rb` as
follows:

```bash
bundle exec trema run lib/routing_switch.rb -c trema.conf
```

To run with virtual slicing support, run `lib/routing_switch.rb` with
`-- --slicing` options as follows:

```bash
bundle exec trema run lib/routing_switch.rb -c trema.conf -- --slicing
```

In another terminal, you can create virtual slices with the following
command:

```bash
bundle exec ./bin/slice add foo
```

Then add hosts to the slice with the following command:

```bash
bundle exec ./bin/slice add_host --mac 11:11:11:11:11:11 --port 0x1:1 --slice foo
```


REST API
--------

To start the REST API server:

```bash
bundle exec rackup
```

## Supported APIs

Description    | Method | URI
---------------|--------|--------------------
Create a slice | POST   | `/slices`
Delete a slice | DELETE | `/slices`
List slices    | GET    | `/slices`
Shows a slice  | GET    | `/slices/:slice_id`
