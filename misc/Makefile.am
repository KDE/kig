INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libmisc.la
noinst_HEADERS=type.h types.h objects.h hierarchy.h coordinates.h
libmisc_la_SOURCES=type.cpp types.cpp objects.cpp hierarchy.cpp coordinates.cc
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -j -o $(podir)/kig.pot
METASOURCES=AUTO
