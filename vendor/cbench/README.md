cbench
======
[![Build Status](http://img.shields.io/travis/trema/cbench/develop.svg?style=flat)][travis]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/cbench.svg?style=flat)][codeclimate]
[![Coverage Status](http://img.shields.io/coveralls/trema/cbench/develop.svg?style=flat)][coveralls]
[![Dependency Status](http://img.shields.io/gemnasium/trema/cbench.svg?style=flat)][gemnasium]

An dedicated OpenFlow controller implementation for "cbench" OpenFlow
controller benchmark.

[travis]: http://travis-ci.org/trema/cbench
[codeclimate]: https://codeclimate.com/github/trema/cbench
[coveralls]: https://coveralls.io/r/trema/cbench
[gemnasium]: https://gemnasium.com/trema/cbench


Prerequisites
-------------

* Ruby 2.0.0 or higher ([RVM][rvm]).
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).

[rvm]: https://rvm.io/
[openvswitch]: https://openvswitch.org/


Install
-------

```
$ git clone https://github.com/trema/cbench.git
$ cd cbench
$ bundle install --binstubs
```


Play
----

Run this controller

```
$ ./bin/trema run ./lib/cbench.rb
```

then, on another terminal

```
$ ./bin/cbench --port 6653 --switches 1 --loops 10 --ms-per-test 10000 --delay 1000 --throughput
```

Enjoy!
