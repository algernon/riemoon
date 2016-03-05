# -*- Autoconf -*-

AC_INIT([riemoon], [0.0.1], [algernon@madhouse-project.org])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-xz no-dist-gzip])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([lib/riemoon.c])
AC_CONFIG_HEADERS([lib/platform.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_CXX

LT_INIT([shared])

AC_CHECK_HEADERS([])
AC_CHECK_FUNCS([])
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Check for pkg-config-enabled libraries
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES(LUA, lua5.3,,[
 PKG_CHECK_MODULES(LUA, lua5.2,,[
  PKG_CHECK_MODULES(LUA, lua5.1,,[
   PKG_CHECK_MODULES(LUA, luajit,,[
   AC_MSG_ERROR([Package requirements (lua) were not met:

No package 'lua5.3', 'lua5.2', 'lua5.1' or 'luajit' found.

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.

Alternatively, you may set the environment variables LUA_CFLAGS
and LUA_LIBS to avoid the need to call pkg-config.
See the pkg-config man page for more details.])
   ])]
  )]
 )]
)
PKG_CHECK_MODULES(RIEMANN_C, riemann-client >= 1.4.0)

AC_CONFIG_FILES([
        Makefile
        riemoon.rockspec
])
AC_CONFIG_LINKS([
        riemoon-${VERSION}-1.rockspec:riemoon.rockspec
])
AC_OUTPUT
