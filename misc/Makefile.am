INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libmisc.la
noinst_HEADERS = common.h coordinate.h coordinate_system.h \
	kigpainter.h objects.h rect.h 
	screeninfo.h calcpaths.h conic-common.h \
	kignumerics.h argsparser.h kigtransform.h \
	guiaction.h object_constructor.h lists.h \
	builtin_stuff.h cubic-common.h \
	special_constructors.h object_hierarchy.h

libmisc_la_SOURCES = common.cpp coordinate_system.cpp \
	kigpainter.cpp objects.cpp \
	screeninfo.cc calcpaths.cc rect.cc coordinate.cpp \
	conic-common.cpp kignumerics.cpp argsparser.cpp \
	kigtransform.cpp guiaction.cc object_constructor.cc \
	lists.cc builtin_stuff.cc cubic-common.cc \
	special_constructors.cc object_hierarchy.cc

libmisc_la_LIBADD=-lm
METASOURCES=AUTO
