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

#include <lauxlib.h>

typedef struct
{
  riemann_client_t *client;
} riemoon_client_t;

typedef struct
{
  riemann_message_t *message;
} riemoon_response_t;

typedef struct
{
  riemann_event_t *data;
} riemoon_event_t;

static int
riemoon_destroy (lua_State *l)
{
  riemoon_client_t *client;

  client = (riemoon_client_t *)luaL_checkudata (l, 1, "Riemoon.Client");

  riemann_client_free (client->client);

  return 0;
}

static int
riemoon_response_destroy (lua_State *l)
{
  riemoon_response_t *response;

  response = (riemoon_response_t *)luaL_checkudata (l, 1, "Riemoon.Response");

  riemann_message_free (response->message);

  return 0;
}

static riemann_event_t *
_riemoon_event_create (lua_State *l)
{
  riemann_event_t *event;

  event = riemann_event_new ();

  lua_pushnil (l);
  while (lua_next (l, -2) != 0) {
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

    lua_pop (l, 1);
  }

  return event;
}

static int
riemoon_send (lua_State *l)
{
  riemoon_client_t *client;
  riemann_message_t *message;
  int i, count, r;

  client = (riemoon_client_t *)luaL_checkudata (l, 1, "Riemoon.Client");

  message = riemann_message_new ();

  count = lua_gettop (l) - 1;

  for (i = 1; i <= count; i++)
    {
      riemann_event_t *event;

      luaL_checktype (l, 2, LUA_TTABLE);
      event = _riemoon_event_create (l);
      riemann_message_append_events (message, event, NULL);

      lua_pop (l, 1);
    }

  r = riemann_client_send_message_oneshot (client->client, message);

  lua_settop (l, 0);

  lua_pushinteger (l, -r);
  lua_pushstring (l, strerror (-r));

  return 2;
}

static int
riemoon_query (lua_State *l)
{
  riemoon_client_t *client;
  riemoon_response_t *resp;
  riemann_message_t *response;
  const char *query;
  int r;

  client = (riemoon_client_t *)luaL_checkudata (l, 1, "Riemoon.Client");
  query = luaL_checkstring (l, 2);

  r = riemann_client_send_message_oneshot
    (client->client,
     riemann_message_create_with_query (riemann_query_new (query)));

  if (r != 0)
    {
      lua_settop (l, 0);

      lua_pushnil (l);
      lua_pushinteger (l, -r);
      lua_pushstring (l, strerror (-r));

      return 3;
    }

  response = riemann_client_recv_message (client->client);
  if (!response)
    {
      lua_settop (l, 0);

      lua_pushnil (l);
      lua_pushinteger (l, errno);
      lua_pushstring (l, strerror (errno));

      return 3;
    }

  if (response->ok != 1)
    {
      lua_settop (l, 0);

      lua_pushnil (l);
      lua_pushinteger (l, -1);
      lua_pushstring (l, response->error);

      return 3;
    }

  resp = (riemoon_response_t *)lua_newuserdata (l, sizeof (riemoon_response_t));
  resp->message = response;

  luaL_setmetatable (l, "Riemoon.Response");

  lua_pushinteger (l, -r);
  lua_pushstring (l, strerror (-r));

  return 3;
}

static int
riemoon_connect (lua_State *l)
{
  riemoon_client_t *ud;
  riemann_client_t *client;
  const char *type_s, *host;
  int port;
  riemann_client_type_t type;

  luaL_argcheck (l, lua_gettop (l) <= 3,
                 1,
                 "expected at most 3 arguments");

  type_s = luaL_optstring (l, 1, "tcp");
  host = luaL_optstring (l, 2, "127.0.0.1");
  port = luaL_optinteger (l, 3, 5555);

  if (strcmp (type_s, "tcp") == 0)
    type = RIEMANN_CLIENT_TCP;
  else if (strcmp (type_s, "udp") == 0)
    type = RIEMANN_CLIENT_UDP;
  else
    luaL_error (l, "invalid riemann client type: %s", type_s);

  lua_settop (l, 0);

  client = riemann_client_create (type, host, port);
  if (!client)
    {
      lua_pushnil (l);
      lua_pushinteger (l, errno);
      lua_pushstring (l, strerror (errno));
      return 3;
    }

  lua_newtable (l);

  ud = (riemoon_client_t *)lua_newuserdata (l, sizeof (riemoon_client_t));
  ud->client = client;

  luaL_getmetatable (l, "Riemoon.Client");
  lua_setmetatable (l, -2);

  lua_pushinteger (l, 0);
  lua_pushstring (l, strerror (0));

  return 3;
}

static int
riemoon_response_len (lua_State *l)
{
  riemoon_response_t *response;

  response = (riemoon_response_t *)luaL_checkudata (l, 1, "Riemoon.Response");

  lua_pushinteger (l, response->message->n_events);
  return 1;
}

static int
riemoon_response_index (lua_State *l)
{
  riemoon_response_t *response;
  riemoon_event_t *event;
  size_t index;

  response = (riemoon_response_t *)luaL_checkudata (l, 1, "Riemoon.Response");
  index = (size_t) luaL_checkinteger (l, 2);

  if (index > response->message->n_events)
    {
      lua_pushnil (l);
      return 1;
    }

  event = (riemoon_event_t *)lua_newuserdata (l, sizeof (riemoon_event_t));
  event->data = response->message->events[index - 1];

  luaL_setmetatable (l, "Riemoon.Event");

  return 1;
}

static int
riemoon_event_index (lua_State *l)
{
  riemoon_event_t *event;
  const char *key;
  size_t i;

  event = (riemoon_event_t *)luaL_checkudata (l, 1, "Riemoon.Event");
  key = luaL_checkstring (l, 2);

  if (strcmp (key, "service") == 0)
    {
      lua_pushstring (l, event->data->service);
      return 1;
    }
  if (strcmp (key, "time") == 0)
    {
      lua_pushinteger (l, (lua_Integer)event->data->time);
      return 1;
    }
  if (strcmp (key, "state") == 0)
    {
      lua_pushstring (l, event->data->state);
      return 1;
    }
  if (strcmp (key, "host") == 0)
    {
      lua_pushstring (l, event->data->host);
      return 1;
    }
  if (strcmp (key, "description") == 0)
    {
      lua_pushstring (l, event->data->description);
      return 1;
    }
  if (strcmp (key, "ttl") == 0)
    {
      lua_pushnumber (l, (lua_Number) event->data->ttl);
      return 1;
    }
  if (strcmp (key, "metric") == 0)
    {
      lua_pushnumber (l, (lua_Number) event->data->metric_d);
      return 1;
    }
  if (strcmp (key, "tags") == 0)
    {
      lua_createtable (l, event->data->n_tags, 0);

      for (i = 0; i < event->data->n_tags; i++)
        {
          lua_pushstring (l, event->data->tags[i]);
          lua_rawseti (l, -2, i + 1);
        }

      return 1;
    }
  if (strcmp (key, "attributes") == 0)
    {
      lua_createtable (l, 0, event->data->n_attributes);

      for (i = 0; i < event->data->n_attributes; i++)
        {
          riemann_attribute_t *a = event->data->attributes[i];

          lua_pushstring (l, a->key);
          lua_pushstring (l, a->value);
          lua_rawset (l, -3);
        }

      return 1;
    }

  for (i = 0; i < event->data->n_attributes; i++)
    {
      riemann_attribute_t *a = event->data->attributes[i];

      if (strcmp (a->key, key) == 0)
        {
          lua_pushstring (l, a->value);
          return 1;
        }
    }

  lua_pushnil (l);
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
    {"query", riemoon_query},
    {"__gc", riemoon_destroy},
    {NULL, NULL}
  };
  luaL_Reg empty_functions[] = {
    {NULL, NULL}
  };

  luaL_newlib (l, empty_functions);
  luaL_newmetatable (l, "Riemoon.Response");

  lua_pushstring (l, "__gc");
  lua_pushcfunction (l, riemoon_response_destroy);
  lua_settable (l, -3);

  lua_pushstring (l, "__len");
  lua_pushcfunction (l, riemoon_response_len);
  lua_settable (l, -3);

  lua_pushstring (l, "__index");
  lua_pushcfunction (l, riemoon_response_index);
  lua_settable (l, -3);

  lua_pop (l, 1);

  luaL_newlib (l, empty_functions);
  luaL_newmetatable (l, "Riemoon.Event");

  lua_pushstring (l, "__index");
  lua_pushcfunction (l, riemoon_event_index);
  lua_settable (l, -3);

  lua_pop (l, 1);

  luaL_newmetatable (l, "Riemoon.Client");
  lua_pushvalue (l, -1);

  lua_setfield (l, -2, "__index");

  luaL_setfuncs (l, methods, 0);

  luaL_newlib (l, functions);

  return 1;
}
