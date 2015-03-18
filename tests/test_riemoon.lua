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

function test_riemann_query ()
   client = riemoon.connect ("tcp", "127.0.0.1", 5555)

   client:send ({service = "riemoon query test",
                 state = "ok",
                 tags = {"riemoon"},
                 riemoon = "yes"})

   client = riemoon.connect ("tcp", "127.0.0.1", 5555)

   data, errno, err = client:query ("error =")
   assertNil (data)
   assertEquals (errno, -1)

   data, errno, err = client:query ("service = \"riemoon query test\"")
   assertNotNil (data)
   assertEquals (errno, 0)
   assertEquals (#data, 1)
   assertEquals (data[1].service, "riemoon query test")
   assertEquals (data[1].state, "ok")
   assertEquals (data[1].riemoon, "yes")
   assertEquals (data[1].tags[1], "riemoon")
   assertEquals (data[1].attributes['riemoon'], "yes")

   data = nil
   collectgarbage ()
end

lu = LuaUnit.new ()
lu:setOutputType ("tap")

r = lu:runSuite ()

collectgarbage ()

os.exit (r)
