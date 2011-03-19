widgetincludedir = $(prefix)/include/kino
gtkincludedir = $(prefix)/include/kino
INCLUDES = -I$(includedir) -I$(top_srcdir)/parser -I$(top_srcdir)/widget

if gtk_found
GTKLIB      = libgtkkino.a
GTKLTLIB    = libgtkkino.la
else
GTKLIB      = 
GTKLTLIB    = 
endif

if x_found
WIDGETLIB   = libkinowidget.a
WIDGETLTLIB = libkinowidget.la
else
WIDGETLIB   = 
WIDGETLTLIB =
endif

lib_LIBRARIES   = $(WIDGETLIB) $(GTKLIB) libnokinowidget.a
lib_LTLIBRARIES = $(WIDGETLTLIB) $(GTKLTLIB) libnokinowidget.la

libnokinowidget_a_SOURCES = NoVisual.c HTMLFormTags.c Style.c
libnokinowidget_la_SOURCES = $(libnokinowidget_a_SOURCES)
libnokinowidget_la_LDFLAGS = -version-info 5:0:3

if gtk_found
libgtkkino_a_SOURCES = \
    gtkkino.c GtkVisual.c Layouter.c Style.c Line.c HTMLFormTags.c

gtkinclude_HEADERS = \
    gtkkino.h HTMLFormTags.h

libgtkkino_la_SOURCES = $(libgtkkino_a_SOURCES)

libgtkkino_la_LDFLAGS = -version-info 5:0:3
endif

if x_found
libkinowidget_a_SOURCES = \
    Kino.c        Style.c        Event.c      HTMLFormTags.c \
    Painter.c     Layouter.c    \
    Line.c        XVisual.c

widgetinclude_HEADERS = \
    Kino.h      Layouter.h  Painter.h   \
    Event.h     KinoP.h     Line.h      Style.h     \
    Visual.h HTMLFormTags.h

libkinowidget_la_SOURCES = $(libkinowidget_a_SOURCES)

libkinowidget_la_LDFLAGS = -version-info 5:0:3
endif
