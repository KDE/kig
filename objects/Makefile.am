INCLUDES=$(all_includes)
noinst_LTLIBRARIES=libobjects.la
noinst_HEADERS=bogus_imp.h common.h line_imp.h \
	object_imp_factory.h object_imp.h object_factory.h \
	object.h point_imp.h line_type.h \
	object_type.h property.h curve_imp.h circle_imp.h \
	conic_imp.h base_type.h circle_type.h conic_types.h \
	cubic_imp.h cubic_type.h object_type_factory.h other_imp.h \
	other_type.h intersection_types.h locus_imp.h \
	custom_types.h transform_types.h text_imp.h
libobjects_la_SOURCES=bogus_imp.cc line_imp.cc object.cc \
	object_imp_factory.cc object_factory.cc point_imp.cc \
	line_type.cc object_type.cc property.cc object_imp.cc \
	point_type.cc curve_imp.cc circle_imp.cc conic_imp.cc \
	base_type.cc circle_type.cc conic_types.cc cubic_imp.cc \
	cubic_type.cc object_type_factory.cc other_imp.cc \
	other_type.cc intersection_types.cc locus_imp.cc \
	custom_types.cc transform_types.cc text_imp.cc
libobjects_la_LIBADD=-lm
METASOURCES=AUTO
