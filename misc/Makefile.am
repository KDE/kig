INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libmisc.la
noinst_HEADERS = common.h coordinate.h coordinate_system.h hierarchy.h \
	kigpainter.h objects.h rect.h \
	screeninfo.h calcpaths.h conic-common.h \
	kignumerics.h argsparser.h kigtransform.h \
	guiaction.h object_constructor.h object_constructor_list.h \
	builtin_stuff.h

libmisc_la_SOURCES = common.cpp coordinate_system.cpp hierarchy.cpp \
	kigpainter.cpp objects.cpp \
	screeninfo.cc calcpaths.cc rect.cc coordinate.cpp \
	conic-common.cpp kignumerics.cpp argsparser.cpp \
	kigtransform.cpp guiaction.cc object_constructor.cc \
	object_constructor_list.cc builtin_stuff.cc

libmisc_la_LIBADD=-lm
METASOURCES=AUTO
