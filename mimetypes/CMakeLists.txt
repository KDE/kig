install(FILES
   x-kig.desktop
   x-kgeo.desktop
   x-kseg.desktop
   x-cabri.desktop
   x-drgeo.desktop
   DESTINATION ${MIME_INSTALL_DIR}/application)
install(FILES
   cabri.magic
   drgeo.magic
   DESTINATION ${CONFIG_INSTALL_DIR}/magic)

kde4_install_icons( ${ICON_INSTALL_DIR}  crystalsvg )


#original Makefile.am contents follow:

## mime types...
#kdemime_DATA = x-kig.desktop x-kgeo.desktop x-kseg.desktop x-cabri.desktop x-drgeo.desktop
#kdemimedir=$(kde_mimedir)/application
#
#magicdir = $(kde_confdir)/magic
#magic_DATA = cabri.magic drgeo.magic
#
#KDE_ICON = AUTO
