SUBDIRS=native kgeo cabri

INCLUDES=$(all_includes)

METASOURCES = AUTO

noinst_LTLIBRARIES=libfilters.la
noinst_HEADERS= \
	filter.h \
	exporter.h \
	exporttoimagedialog.h 
libfilters_la_SOURCES= \
	filter.cc \
	exporter.cc \
	exporttoimagedialogbase.ui \
	exporttoimagedialog.cc
libfilters_la_LIBADD=native/libnative.la kgeo/libkgeo.la cabri/libcabri.la
