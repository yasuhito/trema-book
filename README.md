hello_trema
===========

[![Build Status](http://img.shields.io/travis/trema/hello_trema/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/hello_trema/develop.svg?style=flat)][coveralls]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/hello_trema.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/hello_trema.svg?style=flat)][gemnasium]
[![Inline docs](http://inch-ci.org/github/trema/hello_trema.png?branch=develop)][inch]

The "Hello Trema!" example is one of the simplest OpenFlow controller
implementation. The basic functionality of this controller is to
establish an OpenFlow channel connection with an OpenFlow switch and
output the `"Hello 0xabc! (switch's datapath ID)"` message.

This demonstrates a minimum template for Trema applications written in
Ruby. Hence it's a good starting point to learn about Trema
programming.

[travis]: http://travis-ci.org/trema/hello_trema
[coveralls]: https://coveralls.io/r/trema/hello_trema
[codeclimate]: https://codeclimate.com/github/trema/hello_trema
[gemnasium]: https://gemnasium.com/trema/hello_trema
[inch]: http://inch-ci.org/github/trema/hello_trema


Prerequisites
-------------

* Ruby 2.0.0 or higher ([RVM][rvm]).
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).

[rvm]: https://rvm.io/
[openvswitch]: https://openvswitch.org/


Install
-------

```
$ git clone https://github.com/trema/hello_trema.git
$ cd hello_trema
$ bundle install
```


Play
----

```
$ bundle exec trema run lib/hello_trema.rb -c trema.conf -- foo bar baz
Trema started (args = ["foo", "bar", "baz"]).
Hello 0xabc!
```
