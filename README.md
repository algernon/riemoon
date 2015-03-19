Riemoon
=======

[![Build Status](https://img.shields.io/travis/algernon/riemoon/master.svg?style=flat-square)](https://travis-ci.org/algernon/riemoon)

This is a [Riemann][riemann] client library for the [Lua][lua]
programming language, built on top of [riemann-c-client][rcc]. For
now, it's a work in progress library.

 [riemann]: http://riemann.io/
 [lua]: http://lua.org/
 [rcc]: https://github.com/algernon/riemann-c-client

The library uses [semantic versioning][semver].

 [semver]: http://semver.org/

Installation
------------

The library requires [lua][lua] >= 5.2, [riemann-c-client][rcc] >=
1.4.0, autotools and [busted][busted] to build. It is recommended to
install and use the library via [LuaRocks][luarocks]:

    $ luarocks install riemoon

 [busted]: http://olivinelabs.com/busted/
 [luarocks]: http://luarocks.org/

Demo
----

A simple program that sends a static event to [Riemann][riemann] is
included below. More examples can be found in the [test suite][tests].

 [tests]: https://github.com/algernon/riemoon/tree/master/tests

```lua
riemoon = require ("riemoon")

client = riemoon.connect ()
client:send ({host = "localhost",
              service = "demo-client",
              state =" ok",
              tags = {"demo-client", "riemoon"},
              riemoon = "0.0.0"})
```

License
-------

Copyright (C) 2015 Gergely Nagy <algernon@madhouse-project.org>,
released under the terms of the
[GNU Lesser General Public License][lgpl], version 3+.

 [lgpl]: http://www.gnu.org/licenses/lgpl.html
