patch_panel
===========

[![Build Status](http://img.shields.io/travis/trema/patch_panel/develop.svg?style=flat)][travis]
[![Coverage Status](http://img.shields.io/coveralls/trema/patch_panel/develop.svg?style=flat)][coveralls]
[![Code Climate](http://img.shields.io/codeclimate/github/trema/patch_panel.svg?style=flat)][codeclimate]
[![Dependency Status](http://img.shields.io/gemnasium/trema/patch_panel.svg?style=flat)][gemnasium]
[![Inline docs](http://inch-ci.org/github/trema/patch_panel.png?branch=develop)][inch]

OpenFlow controller that emulates a software patch panel.

[travis]: http://travis-ci.org/trema/patch_panel
[coveralls]: https://coveralls.io/r/trema/patch_panel
[codeclimate]: https://codeclimate.com/github/trema/patch_panel
[gemnasium]: https://gemnasium.com/trema/patch_panel
[inch]: http://inch-ci.org/github/trema/patch_panel


Prerequisites
-------------

* Ruby 2.0.0 or higher ([RVM][rvm]).
* [Open vSwitch][openvswitch] (`apt-get install openvswitch-switch`).

[rvm]: https://rvm.io/
[openvswitch]: https://openvswitch.org/


Install
-------

```
$ git clone https://github.com/trema/patch_panel.git
$ cd patch_panel
$ bundle install
```


Play
----

```
% bundle exec trema run lib/patch_panel.rb -c trema.conf
```


Configuration
-------------

When `patch_panel.rb` starts up, it will search for its configuration
file named `patch_panel.conf` in the current working directory. If you
want to specify a path to a configuration file, pass it to
`patch_panel.rb` as its argument:

```
% bundle exec trema run lib/patch_panel.rb -c trema.conf -- foobar.conf
```

A configuration file contains patching information. Each line has a
pair of port numbers like so:

```
1 2
3 4
```

The first line means that switch port #1 is connected to port #2.
