Learning Switch
===============

[![Build Status](http://img.shields.io/travis/trema/learning_switch/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/learning_switch/develop.svg?style=flat)][coveralls]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/learning_switch.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/learning_switch.svg?style=flat)][gemnasium]

An OpenFlow controller that controls a single OpenFlow switch and
emulates a layer 2 switch.

[travis]: http://travis-ci.org/trema/learning_switch
[coveralls]: https://coveralls.io/r/trema/learning_switch
[codeclimate]: https://codeclimate.com/github/trema/learning_switch
[gemnasium]: https://gemnasium.com/trema/learning_switch

Install
-------

```
$ git clone https://github.com/trema/learning_switch.git
$ cd learning_switch
$ bundle install
```


Play
----

```
% bundle exec trema run lib/learning_switch.rb -c trema.conf
```
