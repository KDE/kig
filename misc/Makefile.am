INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libmisc.la
noinst_HEADERS = common.h coordinate.h coordinate_system.h hierarchy.h \
		type.h types.h kigpainter.h objects.h rect.h \
		screeninfo.h calcpaths.h
libmisc_la_SOURCES = common.cpp coordinate_system.cpp hierarchy.cpp \
		type.cpp types.cpp kigpainter.cpp objects.cpp \
		screeninfo.cc calcpaths.cc
libmisc_la_LIBADD=-lm
METASOURCES=AUTO
