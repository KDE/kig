kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kig FILES tips )

kde4_footer()



#original Makefile.am contents follow:

#appdatadir = $(kde_datadir)/kig
#
#appdata_DATA = tips
