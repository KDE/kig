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
