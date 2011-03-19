INCLUDES = $(x_includes) -I$(includedir) -I$(top_srcdir)/parser -I$(top_srcdir)/ahd -I$(top_srcdir)/widget

if lua_found
LUALIBS = -lkinoahdlua -lkinoparserlua -lkinowidgetlua \
	-llua -lloadlib -llualib -lposlib -ldl -lm
else
LUALIBS =
endif

if tcl_found
TCLLIBS = -lkinoahdtcl -lkinoparsertcl -lkinowidgettcl \
	-ltcl8.0 -ldl -lm
else
TCLLIBS =
endif

if pthread_found
THREADLIBS = -lpthread
else
THREADLIBS =
endif

XLIBS = -lXp -lXaw -lXt -lX11

noinst_PROGRAMS = parsertest widgettest ahdruntime gtktest atlas

widgettest_SOURCES = widgettest.c
if dmalloc
widgettest_LDFLAGS = $(X_LIBS) -L$(libdir) $(XLIBS) \
	$(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	-lkinoparser -lkinoahd -lkinowidget \
	-ldmalloc
else
widgettest_LDFLAGS = $(X_LIBS) -L$(libdir) $(XLIBS) \
	$(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	-lkinoparser -lkinoahd -lkinowidget
endif

gtktest_SOURCES = gtktest.c
if dmalloc
gtktest_LDFLAGS = $(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	`gtk-config --libs` -ldmalloc
else
gtktest_LDFLAGS = $(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	`gtk-config --libs`
endif
gtktest_LDADD = ../widget/libgtkkino.a

parsertest_SOURCES = parsertest.c
if dmalloc
parsertest_LDFLAGS = $(THREADLIBS) -ldmalloc
else
parsertest_LDFLAGS = $(THREADLIBS)
endif
parsertest_LDADD = ../parser/libkinoparser.a

ahdruntime_SOURCES = ahdruntime.c
if dmalloc
ahdruntime_LDFLAGS = $(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	-lnokinowidget -ldmalloc
else
ahdruntime_LDFLAGS = $(LUALIBS) $(TCLLIBS) \
	-lnokinowidget
endif

atlas_SOURCES = atlas.c
if dmalloc
atlas_LDFLAGS = $(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	`gtk-config --libs` -ldmalloc
else
atlas_LDFLAGS = $(LUALIBS) $(TCLLIBS) $(THREADLIBS) \
	`gtk-config --libs`
endif
atlas_LDADD = ../widget/libgtkkino.a

