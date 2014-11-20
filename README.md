cbench
======
[![Build Status](http://img.shields.io/travis/trema/cbench/develop.svg?style=flat)][travis]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/cbench.svg?style=flat)][codeclimate]

An dedicated OpenFlow controller implementation for "cbench" OpenFlow
controller benchmark.

[travis]: http://travis-ci.org/trema/cbench
[codeclimate]: https://codeclimate.com/github/trema/cbench

Install
-------

```bash
$ git clone https://github.com/trema/cbench.git
$ cd cbench
$ bundle install
$ bundle exec rake
```


Play
----

Run this controller

```bash
% bundle exec trema run ./lib/cbench_switch.rb
```

then, on another terminal

```bash
% ./vendor/oflops-0.03.trema1/cbench/cbench --port 6653 --switches 1 --loops 10 --delay 1000
```

Enjoy!
