
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


# kig

set(kig_SRCS
  main.cpp
  kig.cpp
)

kde4_automoc(${kig_SRCS})

kde4_add_executable(kig ${kig_SRCS})

target_link_libraries(kig  ${KDE4_KPARTS_LIBS} )

install(TARGETS kig DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES kig.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kig_part.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kigui.rc kigpartui.rc DESTINATION ${DATA_INSTALL_DIR}/kig )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

