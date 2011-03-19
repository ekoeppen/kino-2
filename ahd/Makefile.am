kinoincludedir = $(prefix)/include/kino

INCLUDES = -I$(top_srcdir)/parser -I$(top_srcdir)/ahd -I$(top_srcdir)/widget -I$(includedir)

lib_LIBRARIES = libkinoahd.a
lib_LTLIBRARIES = libkinoahd.la

if native_networking
kinoinclude_HEADERS = \
        AHDRuntime.h Net.h

libkinoahd_a_SOURCES = \
        AHDRuntime.c Net.c
else
kinoinclude_HEADERS = \
        AHDRuntime.h

libkinoahd_a_SOURCES = \
        AHDRuntime.c

endif

libkinoahd_la_SOURCES = $(libkinoahd_a_SOURCES)

if pthread_found
libkinoahd_la_LDFLAGS = -lpthread -version-info 5:0:3
else
libkinoahd_la_LDFLAGS = -version-info 5:0:3
endif
