SUBDIRS = objects misc modes icons filters examples kig mimetypes macros

kde_module_LTLIBRARIES = libkigpart.la
libkigpart_la_SOURCES = dummy.cpp
libkigpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkigpart_la_LIBADD  = $(LIB_KPARTS) \
	misc/libmisc.la objects/libobjects.la filters/libfilters.la \
	modes/libmodes.la kig/libkigparttemp.la

messages: rc.cpp
	$(EXTRACTRC) *.rc */*.rc >> rc.cpp
	$(EXTRACTRC) */*.ui >> rc.cpp
	for file in macros/*.kigt; do \
		cat "$$file" | grep '<Name>' | sed -e 's/^ *<Name>\([^<]*\)<\/Name>/i18n( "\1" );/' >> rc.cpp; \
		cat "$$file" | grep '<Description>' | sed -e 's/^ *<Description>\([^<]*\)<\/Description>/i18n( "\1" );/' >> rc.cpp; \
	done
	$(XGETTEXT) -ki18n -kI18N_NOOP rc.cpp */*.h */*.cc */*.cpp -o $(podir)/kig.pot

dummy.cpp: 
	touch dummy.cpp

TAGS: 
	find -name '*.cc' -o -name '*.cpp' -o -name '*.h' | etags -
