cbench
======
[![Build Status](http://img.shields.io/travis/trema/cbench/develop.svg?style=flat)][travis]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/cbench.svg?style=flat)][codeclimate]
[![Coverage Status](http://img.shields.io/coveralls/trema/cbench/develop.svg?style=flat)][coveralls]
[![Dependency Status](http://img.shields.io/gemnasium/trema/cbench.svg?style=flat)][gemnasium]
[![Gitter chat](https://badges.gitter.im/Join Chat.svg)][gitter]

An dedicated OpenFlow controller implementation for "cbench" OpenFlow
controller benchmark.

[travis]: http://travis-ci.org/trema/cbench
[codeclimate]: https://codeclimate.com/github/trema/cbench
[coveralls]: https://coveralls.io/r/trema/cbench
[gemnasium]: https://gemnasium.com/trema/cbench
[gitter]: https://gitter.im/trema/cbench

Install
-------

```
$ git clone https://github.com/trema/cbench.git
$ cd cbench
$ bundle install
$ bundle exec rake
```


Play
----

Run this controller

```
% bundle exec trema run ./lib/cbench_switch.rb
```

then, on another terminal

```
% bundle exec ./bin/cbench --port 6633 --switches 1 --loops 10 --ms-per-test 10000 --delay 1000
```

Enjoy!
