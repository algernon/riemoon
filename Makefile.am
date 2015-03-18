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
TESTS				= tests/test_riemoon.lua

# -- Extra files to distribute --
EXTRA_DIST			= README.md NEWS GPL LGPL

# -- Custom targets --
coverage: coverage.info.html

coverage.info.html: coverage.info
	$(AM_V_GEN)genhtml --quiet $^ --output-directory $@

coverage.info: check
	$(AM_V_GEN)lcov --quiet --capture --directory ${top_builddir}/lib --output $@ -b ${top_builddir}

.PHONY: coverage
CLEANFILES			= coverage.info

clean-local:
	rm -rf coverage.info.html
	find ${top_builddir} -name '*.gcda' -or -name '*.gcno' | xargs rm -f
