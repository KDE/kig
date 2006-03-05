kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES=$(all_includes)
#
#noinst_LTLIBRARIES=libmodes.la
#libmodes_la_SOURCES= \
#	base_mode.cc \
#	construct_mode.cc \
#	dragrectmode.cc \
#	edittype.cc \
#	edittypewidget.ui \
#	label.cc \
#	linkslabel.cpp \
#	macro.cc \
#	macrowizard.cc \
#	macrowizardbase.ui \
#	mode.cc \
#	moving.cc \
#	normal.cc \
#	popup.cc \
#	textlabelwizard.cc \
#	textlabelwizardbase.ui \
#	typesdialog.cpp \
#	typeswidget.ui
#noinst_HEADERS=\
#	base_mode.h \
#	construct_mode.h \
#	dragrectmode.h \
#	edittype.h \
#	label.h \
#	linkslabel.h \
#	macro.h \
#	macrowizard.h \
#	mode.h \
#	moving.h \
#	normal.h \
#	popup.h \
#	textlabelwizard.h \
#	typesdialog.h
#
#METASOURCES=AUTO
