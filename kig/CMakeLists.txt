
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


########### next target ###############

set(kig_SRCS main.cpp kig.cpp )

kde4_automoc(${kig_SRCS})

kde4_add_executable(kig ${kig_SRCS})

target_link_libraries(kig  ${KDE4_KPARTS_LIBS} )

install(TARGETS kig DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES kig.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kig_part.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kigui.rc kigpartui.rc DESTINATION ${DATA_INSTALL_DIR}/kig )

kde4_install_icons( ${ICON_INSTALL_DIR}   )




#original Makefile.am contents follow:

## this has all of the subdirectories that make will recurse into.  if
## there are none, comment this out
##SUBDIRS = pics icons
#
## set the include path for X, qt and KDE
#INCLUDES = $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS = kig.h kig_document.h kig_part.h kig_view.h kig_commands.h
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#KDE_ICON = AUTO
#
## this Makefile creates both a KPart application and a KPart
##########################################################################
## APPLICATION SECTION
##########################################################################
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = kig
#
## the application source, library search path, and link libraries
#kig_SOURCES = main.cpp kig.cpp
#kig_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#kig_LDADD   = $(LIB_KPARTS)
#
## this is where the desktop file will go 
#xdg_apps_DATA = kig.desktop
#
## this is where the shell's XML-GUI resource file goes
#rcdir   = $(kde_datadir)/kig
#rc_DATA = kigui.rc kigpartui.rc
#
##########################################################################
## KPART SECTION
##########################################################################
#noinst_LTLIBRARIES = libkigparttemp.la
#
## the Part's source, library search path, and link libraries
#libkigparttemp_la_SOURCES = \
#	kig_document.cc \
#	kig_part.cpp \
#	kig_view.cpp \
#	kig_commands.cpp
#
#libkigparttemp_la_LDFLAGS = $(all_libraries)
#libkigparttemp_la_LIBADD = $(LIB_KDEPRINT) $(LIB_KIO)
#
## this is where the desktop file will go 
#partdesktopdir   = $(kde_servicesdir)
#partdesktop_DATA = kig_part.desktop
