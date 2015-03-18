Riemoon
=======

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

The library follows the usual autotools way of installation, and needs
lua 5.2+ and riemann-c-client 1.4.0+ (and libtool 2.2+ to build from
git):

    $ git clone git://github.com/algernon/riemoon.git
    $ cd riemoon
    $ autoreconf -i
    $ ./configure && make && make check && make install

From this point onward, the library is installed and fully functional,
and one can use it from Lua.

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
