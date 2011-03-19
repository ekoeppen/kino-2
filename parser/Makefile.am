kinoincludedir = $(prefix)/include/kino

lib_LIBRARIES = libkinoparser.a
lib_LTLIBRARIES = libkinoparser.la

if pthread_found
THREADLIBS = -lpthread
else
THREADLIBS =
endif

INCLUDES = -I$(top_srcdir)/parser -I$(top_srcdir)/ahd -I$(includedir)

kinoinclude_HEADERS = \
        Attribute.h     KinoParser.h    Parser.h        TableTags.h     \
        Entity.h        KinoUtil.h      Property.h      Handler.h       \
        StyleBase.h     LineBase.h 	HTMLTags.h	XMLTags.h	\
        DOM.h           Table.h         

libkinoparser_a_SOURCES = \
        Attribute.c     HTMLTags.c      Parser.c        XMLTags.c	\
        Table.c         DOM.c		Entity.c        LineBase.c	\
        StyleBase.c     TableTags.c     Handler.c       

libkinoparser_la_SOURCES = $(libkinoparser_a_SOURCES)
libkinoparser_la_LDFLAGS = -version-info 5:0:3 $(THREADLIBS)
