INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libobjects.la
noinst_HEADERS=object.h point.h curve.h circle.h segment.h curve.template.h object.template.h line.h collection.h macro.h intersection.h locus.h
EXTRA_DIST=curve.template.cpp object.template.cpp
libobjects_la_SOURCES=point.cpp object.cpp circle.cpp segment.cpp line.cpp collection.cpp macro.cpp intersection.cpp locus.cpp
libobjects_la_LIBADD=-lm
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -j -o $(podir)/kig.pot
METASOURCES=AUTO
