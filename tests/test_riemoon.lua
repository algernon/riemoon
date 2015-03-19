#! /usr/bin/env busted

fn, err = package.loadlib ("lib/.libs/riemoon.so", "luaopen_riemoon")
if not fn then print (err)
else
   riemoon = fn()
end

describe (
   "Riemoon /",
   function ()
      describe (
         "Connecting /",
         function ()
            it (
               "Should fail to connect to a bad port",
               function ()
                  client, errno, err = riemoon.connect ("tcp", "127.0.0.1", 6555)
                  assert.is_nil (client)
                  assert.are.equal (errno, 111)
            end)

            it (
               "Should connect to Riemann",
               function ()
                  client, errno = riemoon.connect ("tcp", "127.0.0.1", 5555)
                  assert.is_userdata (client)
                  assert.are.equal (errno, 0)
            end)

            it (
               "Should connect with default values",
               function ()
                  client, errno = riemoon.connect ()
                  assert.is_userdata (client)
                  assert.are.equal (errno, 0)
            end)

            it (
               "Should connect with UDP too",
               function ()
                  client, errno, err = riemoon.connect ("udp")
                  assert.is_userdata (client)
                  assert.are.equal (errno, 0)
            end)

            it (
               "Should error out on invalid connection type",
               function ()
                  local errfn = function ()
                     riemoon.connect ("invalid")
                  end
                  assert.has_error (errfn, "invalid riemann client type: invalid")
            end)
      end)

      describe (
         "Sending events /",
         function ()
            client = riemoon.connect ()

            it (
               "Empty events should be ok",
               function ()
                  errno, err = client:send ()
                  assert.are.equal (errno, 0)
            end)

            it (
               "Should be able to send a single event",
               function ()
                  errno, err = client:send({host = "localhost",
                                            service = "lua test",
                                            metric = 1,
                                            tags = {"foo","bar", "baz"},
                                            description = "A test message",
                                            time = 1,
                                            ttl = 60,
                                            something = "else"})
                  assert.are.equal (errno, 0)
            end)

            it (
               "Should be able to send multiple events",
               function ()
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
                  assert.are.equal (errno, 0)
            end)
      end)

      describe (
         "Queries /",
         function ()
            client = riemoon.connect ("tcp", "127.0.0.1", 5555)

            client:send ({service = "riemoon query test",
                          state = "ok",
                          tags = {"riemoon"},
                          riemoon = "yes"})

            client = riemoon.connect ("tcp", "127.0.0.1", 5555)

            it (
               "Should return nil and the error when given an invalid query",
               function ()
                  data, errno, err = client:query ("error =")
                  assert.is_nil (data)
                  assert.are.equal (errno,  -1)
            end)

            it (
               "Should return events when querying for known data",
               function ()
                  data, errno, err = client:query ("service = \"riemoon query test\"")

                  assert.is_not_nil (data)
                  assert.are.equal (errno, 0)
                  assert.are.equal (#data, 1)
                  assert.are.equal (data[1].service, "riemoon query test")
                  assert.are.equal (data[1].state, "ok")
                  assert.are.equal (data[1].riemoon, "yes")
                  assert.are.equal (data[1].tags[1], "riemoon")
                  assert.are.equal (data[1].attributes['riemoon'], "yes")
                  assert.is_nil (data[1].description)
                  assert.is_nil (data[1].host)
                  assert.is_number (data[1].ttl)
                  assert.is_number (data[1].time)
                  assert.is_number (data[1].metric)
                  assert.is_nil (data[1].unknown)
                  assert.is_nil (data[2])
            end)

            it (
               "Should fail to query over UDP",
               function ()
                  client = riemoon.connect ("udp")
                  data, errno, err = client:query ("true")
                  assert.is_nil (data)
                  assert.are.equal (errno, 95)
            end)
      end)
end)
