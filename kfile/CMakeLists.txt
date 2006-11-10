



########### next target ###############

set(kfile_drgeo_PART_SRCS kfile_drgeo.cpp )

kde4_automoc(${kfile_drgeo_PART_SRCS})

kde4_add_plugin(kfile_drgeo ${kfile_drgeo_PART_SRCS})



target_link_libraries(kfile_drgeo  ${KDE4_KIO_LIBS} ${QT_QTXML_LIBRARY} )

install(TARGETS kfile_drgeo DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kfile_kig_PART_SRCS kfile_kig.cpp )

kde4_automoc(${kfile_kig_PART_SRCS})

kde4_add_plugin(kfile_kig ${kfile_kig_PART_SRCS})



target_link_libraries(kfile_kig  ${KDE4_KIO_LIBS} ${QT_QTXML_LIBRARY} )

install(TARGETS kfile_kig DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES
   kfile_drgeo.desktop
   kfile_kig.desktop
   DESTINATION  ${SERVICES_INSTALL_DIR})

