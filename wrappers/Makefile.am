INCLUDES =  -I$(top_srcdir)/parser -I$(top_srcdir)/ahd -I$(top_srcdir)/widget
EXTRA_DIST = kinoparser.i kinoahd.i kinowidget.i

kinoincludedir = $(prefix)/include/kino

if lua_found
LUALIB = libkinoahdlua.a libkinoparserlua.a libkinowidgetlua.a
LUALTLIB = libkinoahdlua.la libkinoparserlua.la libkinowidgetlua.la

ParserLuaWrap.c: kinoparser.i
	swig -strict 0 -module kinoparserlua -Lua -dnone -o $@ $< 

AHDLuaWrap.c: kinoahd.i
	swig -strict 0 -c -module kinoahdlua -Lua -dnone -o $@ $< 

WidgetLuaWrap.c: kinowidget.i
	swig -strict 0 -c -module kinowidgetlua -Lua -dnone -o $@ $< 
endif

if tcl_found
TCLLIB = libkinoahdtcl.a libkinoparsertcl.a libkinowidgettcl.a
TCLLTLIB = libkinoahdtcl.la libkinoparsertcl.la libkinowidgettcl.la

ParserTclWrap.c: kinoparser.i
	swig -strict 0 -module kinoparser -tcl -dnone -o $@ $< 

AHDTclWrap.c: kinoahd.i
	swig -strict 0 -c -module kinoahd -tcl -dnone -o $@ $< 

WidgetTclWrap.c: kinowidget.i
	swig -strict 0 -c -module kinowidget -tcl -dnone -o $@ $< 
endif

lib_LIBRARIES = $(LUALIB) $(TCLLIB)
lib_LTLIBRARIES = $(LUALTLIB) $(TCLLTLIB)


libkinoahdlua_a_SOURCES = AHDLuaWrap.c
libkinoahdlua_la_SOURCES = $(libkinoahdlua_a_SOURCES)
libkinoahdlua_la_LDFLAGS = -lkinoahd -llua -version-info 5:0:3

libkinoahdtcl_a_SOURCES = AHDTclWrap.c
libkinoahdtcl_la_SOURCES = $(libkinoahdtcl_a_SOURCES)
libkinoahdtcl_la_LDFLAGS = -lkinoahd -ltcl8.0 -version-info 5:0:3

libkinowidgetlua_a_SOURCES = WidgetLuaWrap.c
libkinowidgetlua_la_SOURCES = $(libkinowidgetlua_a_SOURCES)
libkinowidgetlua_la_LDFLAGS = -llua -version-info 5:0:3

libkinowidgettcl_a_SOURCES = WidgetTclWrap.c
libkinowidgettcl_la_SOURCES = $(libkinowidgettcl_a_SOURCES)
libkinowidgettcl_la_LDFLAGS = -ltcl8.0 -version-info 5:0:3

libkinoparserlua_a_SOURCES = ParserLuaWrap.c
libkinoparserlua_la_SOURCES = $(libkinoparserlua_a_SOURCES)
libkinoparserlua_la_LDFLAGS = -lkinoparser -llua -version-info 5:0:3

libkinoparsertcl_a_SOURCES = ParserTclWrap.c
libkinoparsertcl_la_SOURCES = $(libkinoparsertcl_a_SOURCES)
libkinoparsertcl_la_LDFLAGS = -lkinoparser -ltcl8.0 -version-info 5:0:3
