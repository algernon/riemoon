/* riemoon -- Lua bindings for riemann-c-client
 * Copyright (C) 2015  Gergely Nagy <algernon@madhouse-project.org>
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <riemann/riemann-client.h>
#include <string.h>
#include <stdio.h>

#define LUA_LIB 1
#include <lua.h>
#include <lauxlib.h>

typedef struct
{
  riemann_client_t *client;
} riemoon_client_t;

static int
riemoon_destroy (lua_State *l)
{
  riemoon_client_t *client;

  client = (riemoon_client_t *)luaL_checkudata (l, 1, "Riemoon");

  if (client->client)
    riemann_client_free (client->client);

  return 0;
}

static int
riemoon_send (lua_State *l)
{
  riemoon_client_t *client;
  riemann_event_t *event;
  riemann_message_t *message;

  client = (riemoon_client_t *)luaL_checkudata (l, 1, "Riemoon");

  luaL_checktype (l, 2, LUA_TTABLE);

  event = riemann_event_new ();

  lua_pushnil(l);
  while (lua_next(l, -2) != 0) {
    const char *key;

    key = lua_tostring (l, -2);

    if (strcmp (key, "time") == 0)
      {
        int64_t value = (int64_t) luaL_checklong (l, -1);

        riemann_event_set_one (event, TIME, value);
      }
    else if (strcmp (key, "state") == 0)
      {
        const char *value = luaL_checkstring (l, -1);

        riemann_event_set_one (event, STATE, value);
      }
    else if (strcmp (key, "service") == 0)
      {
        const char *value = luaL_checkstring (l, -1);

        riemann_event_set_one (event, SERVICE, value);
      }
    else if (strcmp (key, "host") == 0)
      {
        const char *value = luaL_checkstring (l, -1);

        riemann_event_set_one (event, HOST, value);
      }
    else if (strcmp (key, "description") == 0)
      {
        const char *value = luaL_checkstring (l, -1);

        riemann_event_set_one (event, DESCRIPTION, value);
      }
    else if (strcmp (key, "tags") == 0)
      {
        int i, n;

        luaL_checktype (l, -1, LUA_TTABLE);

        n = (int)lua_rawlen (l, -1);

        for (i = 1; i <= n; i++)
          {
            const char *value;

            lua_rawgeti (l, -1, i);
            value = luaL_checkstring (l, -1);

            riemann_event_tag_add (event, value);

            lua_pop (l, 1);
          }

      }
    else if (strcmp (key, "ttl") == 0)
      {
        lua_Number value = luaL_checknumber (l, -1);

        riemann_event_set_one (event, TTL, (float) value);
      }
    else if (strcmp (key, "metric") == 0)
      {
        lua_Number value = luaL_checknumber (l, -1);

        riemann_event_set_one (event, METRIC_D, (double) value);
      }
    else
      {
        const char *value = luaL_checkstring (l, -1);

        riemann_event_attribute_add (event,
                                     riemann_attribute_create(key, value));
      }

    lua_pop(l, 1);
  }

  message = riemann_message_new ();
  riemann_message_set_events (message, event, NULL);

  riemann_client_send_message_oneshot (client->client, message);

  return 0;
}

static int
riemoon_connect (lua_State *l)
{
  riemoon_client_t *client;
  const char *type_s, *host;
  int port;
  riemann_client_type_t type;

  luaL_argcheck (l, lua_gettop (l) <= 3,
                 1,
                 "expected at most 3 arguments");

  type_s = luaL_optstring (l, 1, "tcp");
  host = luaL_optstring (l, 2, "localhost");
  port = luaL_optinteger (l, 3, 5555);

  if (strcmp (type_s, "tcp") == 0)
    type = RIEMANN_CLIENT_TCP;
  else if (strcmp (type_s, "udp") == 0)
    type = RIEMANN_CLIENT_UDP;
  else
    luaL_error (l, "invalid riemann client type: %s", type_s);

  lua_settop (l, 0);

  lua_newtable (l);

  client = (riemoon_client_t *)lua_newuserdata (l, sizeof (riemoon_client_t));
  client->client = riemann_client_create (type, host, port);

  luaL_getmetatable (l, "Riemoon");
  lua_setmetatable (l, -2);

  return 1;
}

int
luaopen_riemoon (lua_State *l)
{
  luaL_Reg functions[] = {
    {"connect", riemoon_connect},
    {NULL, NULL}
  };
  luaL_Reg methods[] = {
    {"send", riemoon_send},
    {"__gc", riemoon_destroy},
    {NULL, NULL}
  };

  luaL_newmetatable (l, "Riemoon");
  lua_pushvalue (l, -1);

  lua_setfield (l, -2, "__index");

  luaL_setfuncs (l, methods, 0);

  luaL_newlib (l, functions);

  return 1;
}
