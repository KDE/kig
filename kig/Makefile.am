# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS = pics icons

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kig.h kig_part.h kig_view.h kig_commands.h kig_iface.h \
		kig_actions.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = kig

# this Makefile creates both a KPart application and a KPart
#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kig

# the application source, library search path, and link libraries
kig_SOURCES = main.cpp kig.cpp kig_iface.cpp kig_iface.skel kig_iface.stub
kig_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kig_LDADD   = $(LIB_KPARTS)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Edutainment
shelldesktop_DATA = kig.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kig
shellrc_DATA = kigui.rc

#########################################################################
# KPART SECTION
#########################################################################
kde_module_LTLIBRARIES = libkigpart.la

# the Part's source, library search path, and link libraries
libkigpart_la_SOURCES = kig_part.cpp kig_view.cpp kig_commands.cpp \
			constructactions.cc kig_actions.cpp \
			exporttoimagedialogbase.ui exporttoimagedialog.cc
libkigpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkigpart_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) ../misc/libmisc.la \
			../objects/libobjects.la ../filters/libfilters.la \
			../modes/libmodes.la -lstdc++

# this is where the desktop file will go 
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kig_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/kigpart
partrc_DATA = kigpartui.rc
