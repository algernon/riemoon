#! /usr/bin/env lua

require ('luaunit')
fn, err = package.loadlib ("lib/.libs/riemoon.so", "luaopen_riemoon")
if not fn then print (err)
else
   riemoon = fn()
end

function test_riemoon_connect ()
   client = riemoon.connect ("tcp", "127.0.0.1", 5555)

   assertIsUserdata (client)
end

function test_riemann_send ()
   client = riemoon.connect ("tcp", "127.0.0.1", 5555)

   client:send({host = "localhost",
                service = "lua test",
                metric = 1,
                tags = {"foo","bar", "baz"},
                something = "else"})
end

lu = LuaUnit.new ()
lu:setOutputType ("tap")

os.exit (lu:runSuite ())
