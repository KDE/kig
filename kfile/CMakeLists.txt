kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_drgeo_PART_SRCS kfile_drgeo.cpp )

kde4_automoc(${kfile_drgeo_PART_SRCS})

kde4_add_plugin(kfile_drgeo ${kfile_drgeo_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_drgeo )

target_link_libraries(kfile_drgeo  ${QT_AND_KDECORE_LIBS} kio )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_drgeo )


########### next target ###############

set(kfile_kig_PART_SRCS kfile_kig.cpp )

kde4_automoc(${kfile_kig_PART_SRCS})

kde4_add_plugin(kfile_kig ${kfile_kig_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_kig )

target_link_libraries(kfile_kig  ${QT_AND_KDECORE_LIBS} kio )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_kig )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_drgeo.desktop  	kfile_kig.desktop )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
#
#noinst_HEADERS   = \
#	kfile_drgeo.h \
#	kfile_kig.h
#
#kde_module_LTLIBRARIES = \
#	kfile_drgeo.la \
#	kfile_kig.la
#
#kfile_drgeo_la_SOURCES = kfile_drgeo.cpp
#kfile_drgeo_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_drgeo_la_LIBADD = $(LIB_KIO)
#
#kfile_kig_la_SOURCES = kfile_kig.cpp
#kfile_kig_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_kig_la_LIBADD = $(LIB_KIO)
#
#METASOURCES = AUTO
#
#services_DATA = \
#	kfile_drgeo.desktop \
#	kfile_kig.desktop
#servicesdir = $(kde_servicesdir)
