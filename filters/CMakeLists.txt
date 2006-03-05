kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES=$(all_includes)
#
#METASOURCES = AUTO
#
#noinst_LTLIBRARIES=libfilters.la
#noinst_HEADERS= \
#	cabri-filter.h \
#	cabri-utils.h \
#	exporter.h \
#	filter.h \
#	filters-common.h \
#	imageexporteroptions.h \
#	kgeo-filter.h \
#	kgeo-resource.h \
#	kseg-defs.h \
#	kseg-filter.h \
#	latexexporter.h \
#	latexexporteroptions.h \
#	native-filter.h \
#	svgexporter.h \
#	svgexporteroptions.h \
#	drgeo-filter.h
#libfilters_la_SOURCES= \
#	cabri-filter.cc \
#	cabri-utils.cc \
#	exporter.cc \
#	filter.cc \
#	filters-common.cc \
#	imageexporteroptions.cc \
#	imageexporteroptionswidget.ui \
#	kgeo-filter.cc \
#	kseg-filter.cc \
#	latexexporter.cc \
#	latexexporteroptions.cc \
#	latexexporteroptionswidget.ui \
#	native-filter.cc \
#	svgexporter.cc \
#	svgexporteroptions.cc \
#	svgexporteroptionswidget.ui \
#	drgeo-filter.cc
#
