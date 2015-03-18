#! /usr/bin/env lua

require ('luaunit')
fn, err = package.loadlib ("lib/.libs/riemoon.so", "luaopen_riemoon")
if not fn then print (err)
else
   riemoon = fn()
end

function test_riemoon_connect ()
   -- Connection failure
   client, errno, err = riemoon.connect ("tcp", "127.0.0.1", 6555)
   assertNil (client)
   assertEquals (errno, 111)

   -- Successful connect
   client, errno = riemoon.connect ("tcp", "127.0.0.1", 5555)
   assertIsUserdata (client)
   assertEquals (errno, 0)

   -- Successful connect, default values
   client, errno = riemoon.connect ()
   assertIsUserdata (client)
   assertEquals (errno, 0)
end

function test_riemann_send ()
   client = riemoon.connect ()

   errno, err = client:send()
   assertEquals (errno, 0)

   errno, err = client:send({host = "localhost",
                             service = "lua test",
                             metric = 1,
                             tags = {"foo","bar", "baz"},
                             something = "else"})
   assertEquals (errno, 0)

   errno, err = client:send({host = "localhost",
                             service = "lua test",
                             metric = 1,
                             tags = {"foo","bar", "baz"},
                             something = "else"},
                            {host = "localhost",
                             service = "lua test #2",
                             metric = 1,
                             tags = {"foo","bar", "baz"},
                             something = "else"})
   assertEquals (errno, 0)
end

lu = LuaUnit.new ()
lu:setOutputType ("tap")

os.exit (lu:runSuite ())
