INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libobjects.la
noinst_HEADERS=object.h point.h curve.h circle.h segment.h line.h \
		macro.h intersection.h locus.h normalpoint.h midpoint.h \
		label.h vector.h translatedpoint.h ray.h mirrorpoint.h \
		arc.h rotatedpoint.h property.h
libobjects_la_SOURCES=point.cpp object.cpp circle.cpp segment.cpp line.cpp \
		macro.cpp intersection.cpp locus.cpp normalpoint.cc \
		midpoint.cc label.cc vector.cc translatedpoint.cc ray.cc \
		mirrorpoint.cc abstractline.cc arc.cc rotatedpoint.cpp \
		property.cc
libobjects_la_LIBADD=-lm
METASOURCES=AUTO
