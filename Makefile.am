if KIG_COMPILE_PYTHON_SCRIPTING
scriptingdir = scripting
scriptinglib = scripting/libscripting.la
else
scriptingdir =
scriptinglib =
endif

SUBDIRS = \
	objects \
	misc \
	modes \
	icons \
	filters \
	examples \
	kig \
	mimetypes \
	macros \
	kfile \
	data \
	pykig \
	$(scriptingdir)

kde_module_LTLIBRARIES = libkigpart.la
libkigpart_la_SOURCES = dummy.cpp
libkigpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkigpart_la_LIBADD  = $(LIB_KPARTS) \
	misc/libmisc.la objects/libobjects.la filters/libfilters.la \
	modes/libmodes.la kig/libkigparttemp.la $(scriptinglib)

messages: rc.cpp
	rm -f tips.cpp
	$(EXTRACTRC) */*.rc >> rc.cpp
	$(EXTRACTRC) */*.ui >> rc.cpp
	(cd data && $(PREPARETIPS) > ../tips.cpp)
	for file in macros/*.kigt; do \
		cat "$$file" | grep '<Name>' | sed -e 's/^ *<Name>\([^<]*\)<\/Name>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
		cat "$$file" | grep '<Description>' | sed -e 's/^ *<Description>\([^<]*\)<\/Description>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
		cat "$$file" | grep '<UseText>' | sed -e 's/^ *<UseText>\([^<]*\)<\/UseText>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
		cat "$$file" | grep '<SelectStatement>' | sed -e 's/^ *<SelectStatement>\([^<]*\)<\/SelectStatement>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
	done
	$(XGETTEXT) tips.cpp rc.cpp filters/*.h kig/*.h misc/*.h modes/*.h objects/*.h scripting/*.h */*.cc kig/*.cpp misc/*.cpp modes/*.cpp -o $(podir)/kig.pot
	$(XGETTEXT) kfile/kfile_drgeo.cpp kfile/kfile_drgeo.h -o $(podir)/kfile_drgeo.pot
	$(XGETTEXT) kfile/kfile_kig.cpp kfile/kfile_kig.h -o $(podir)/kfile_kig.pot

CLEANFILES = dummy.cpp

dummy.cpp: 
	touch dummy.cpp

TAGS: 
	find -name '*.cc' -o -name '*.cpp' -o -name '*.h' | etags -

# Want to do the scripting part separately, but
# include everything else.
DOXYGEN_EXCLUDE = python-scripting-api-dox-mainpage.dox
DOXYGEN_SET_RECURSIVE = YES
include ../admin/Doxyfile.am
