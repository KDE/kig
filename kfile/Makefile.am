INCLUDES         = $(all_includes)

noinst_HEADERS   = \
	kfile_drgeo.h \
	kfile_kig.h

kde_module_LTLIBRARIES = \
	kfile_drgeo.la \
	kfile_kig.la

kfile_drgeo_la_SOURCES = kfile_drgeo.cpp
kfile_drgeo_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_drgeo_la_LIBADD = $(LIB_KIO)

kfile_kig_la_SOURCES = kfile_kig.cpp
kfile_kig_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_kig_la_LIBADD = $(LIB_KIO)

METASOURCES = AUTO

services_DATA = \
	kfile_drgeo.desktop \
	kfile_kig.desktop
servicesdir = $(kde_servicesdir)
