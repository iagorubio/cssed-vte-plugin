## Process this file with automake to produce Makefile.in
plugindir=$(prefix)/lib/cssed/plugins
plugin_PROGRAMS=vte_terminal.so

#the core pplugin files
SRCS=\
vte.c 

if ISDARWIN
vte_terminal_so_SOURCES = $(SRCS)
vte_terminal_so_LDFLAGS =  -bundle -flat_namespace -undefined suppress
vte_terminal_so_LDADD = @PACKAGE_LIBS@
vte_terminal_so_CFLAGS = -fno-common
else
vte_terminal_so_SOURCES= $(SRCS)
vte_terminal_so_LDFLAGS = -shared -fPICS
vte_terminal_so_LDADD = @PACKAGE_LIBS@
endif

INCLUDES=\
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/share/locale"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)/vte-plugin"\" \
	-DPACKAGE_LIB_DIR=\""$(prefix)/lib/cssed/"\" \
	-I$(top_srcdir) -I$(top_srcdir)/src \
	@PACKAGE_CFLAGS@




