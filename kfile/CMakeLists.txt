set(kfile_drgeo_PART_SRCS kfile_drgeo.cpp )
add_library(kfile_drgeo ${kfile_drgeo_PART_SRCS} MODULE)
target_link_libraries(kfile_drgeo KF5::KIO ${QT_QTXML_LIBRARY} )
install(TARGETS kfile_drgeo DESTINATION ${PLUGIN_INSTALL_DIR})

set(kfile_kig_PART_SRCS kfile_kig.cpp )
add_library(kfile_kig ${kfile_kig_PART_SRCS} MODULE)
add_library(kfile_kig KF5::KIO ${QT_QTXML_LIBRARY} )
install(TARGETS kfile_kig DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES
   kfile_drgeo.desktop
   kfile_kig.desktop
   DESTINATION  ${SERVICES_INSTALL_DIR})

