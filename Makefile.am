SUBDIRS = objects misc modes icons filters examples kig mimetypes

kde_module_LTLIBRARIES = libkigpart.la
libkigpart_la_SOURCES = dummy.cpp
libkigpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkigpart_la_LIBADD  = $(LIB_QT) $(LIB_KPARTS) $(LIB_KFILE) \
	misc/libmisc.la objects/libobjects.la filters/libfilters.la \
	modes/libmodes.la kig/libkigparttemp.la -lstdc++

messages: rc.cpp
	$(EXTRACTRC) *.rc */*.rc >> rc.cpp
	$(EXTRACTRC) $(find -name '*.ui') >> rc.cpp
	$(XGETTEXT) -ki18n -kI18N_NOOP rc.cpp */*.h */*.cc */*.cpp -o $(podir)/kig.pot

dummy.cpp: 
	touch dummy.cpp
