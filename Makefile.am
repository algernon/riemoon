# -- Global settings --
AM_MAKEFLAGS			= --no-print-directory
AM_CFLAGS			= -I${top_srcdir}/lib -I${top_builddir}/lib
ACLOCAL_AMFLAGS			= -Im4

VERSION				= @VERSION@${SNAPSHOT_VERSION}

# -- Libraries --
lib_LTLIBRARIES			= lib/riemoon.la

lib_riemoon_la_CFLAGS	= ${AM_CFLAGS} ${LUA_CFLAGS} ${RIEMANN_C_CFLAGS}
lib_riemoon_la_LDFLAGS	= \
	-avoid-version -module
lib_riemoon_la_LIBADD	= ${LUA_LIBS} ${RIEMANN_C_LIBS}
lib_riemoon_la_SOURCES	= \
	lib/riemoon.c

# -- Testcases --
if HAVE_CHECK
UNIT_TESTS			= tests/check_libriemann
TESTS				= ${UNIT_TESTS}

TESTS_ENVIRONMENT		= CK_FORK=no

tests/check_%: CFLAGS += ${CHECK_CFLAGS}
tests/check_%: LDADD += ${CHECK_LIBS} -ldl
tests/check_%: LDFLAGS += -no-install

check_PROGRAMS			= ${TESTS}
endif

check_libriemann_srcs		= \
	tests/mocks.h		  \
	tests/mocks.c		  \
	tests/tests.h		  \
	tests/check_library.c	  \
	tests/check_events.c	  \
	tests/check_messages.c	  \
	tests/check_client.c	  \
	tests/check_attributes.c  \
	tests/check_queries.c	  \
	tests/check_simple.c	  \
	tests/check_libriemann.c

# -- Extra files to distribute --
EXTRA_DIST			= README.md NEWS GPL LGPL \
				  ${check_libriemann_srcs}
