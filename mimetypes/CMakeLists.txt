kde4_header()

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
