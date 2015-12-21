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

```bash
git clone https://github.com/trema/patch_panel.git
cd patch_panel
bundle install --binstubs
```


Play
----

```bash
./bin/trema run lib/patch_panel.rb -c patch_panel.conf -d
```

To create a patch between port #1 and #2 of switch 0xabc:

```bash
./bin/patch_panel create 0xabc 1 2
```

To delete the patch:

```bash
./bin/patch_panel delete 0xabc 1 2
```
