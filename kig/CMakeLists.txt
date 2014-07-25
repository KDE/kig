
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


# kig

set(kig_SRCS
  main.cpp
  kig.cpp
)


#uncomment this if oxygen icons for kig are available
#kde4_add_app_icon(kig_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kig.png")
kde4_add_app_icon(kig_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-kig.png")
add_executable(kig ${kig_SRCS})

target_link_libraries(kig KF5::Parts)

install(TARGETS kig ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( PROGRAMS kig.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kig_part.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kigui.rc kigpartui.rc DESTINATION ${CMAKE_INSTALL_DATADIR}/kig )

ecm_install_icons( ${ICON_INSTALL_DIR}   )

