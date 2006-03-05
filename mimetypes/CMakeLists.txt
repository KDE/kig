kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${MIME_INSTALL_DIR}/application FILES x-kig.desktop x-kgeo.desktop x-kseg.desktop x-cabri.desktop x-drgeo.desktop )
install_files( ${CONFIG_INSTALL_DIR}/magic FILES cabri.magic drgeo.magic )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

## mime types...
#kdemime_DATA = x-kig.desktop x-kgeo.desktop x-kseg.desktop x-cabri.desktop x-drgeo.desktop
#kdemimedir=$(kde_mimedir)/application
#
#magicdir = $(kde_confdir)/magic
#magic_DATA = cabri.magic drgeo.magic
#
#KDE_ICON = AUTO
