SUBDIRS=native kgeo cabri

INCLUDES=$(all_includes)

noinst_LTLIBRARIES=libfilters.la
noinst_HEADERS=filter.h
libfilters_la_SOURCES=filter.cc
libfilters_la_LIBADD=native/libnative.la kgeo/libkgeo.la cabri/libcabri.la 
