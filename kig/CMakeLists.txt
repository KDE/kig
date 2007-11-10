
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


# kig

set(kig_SRCS
  main.cpp
  kig.cpp
)


#uncomment this if oxygen icons for kig are available
#set(kig_icons
#    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/16x16/apps/kig.png
#    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/32x32/apps/kig.png
#    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/48x48/apps/kig.png
#    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/64x64/apps/kig.png
#    ${CMAKE_INSTALL_PREFIX}/share/icons/oxygen/128x128/apps/kig.png
#)
set(kig_icons
    ${CMAKE_CURRENT_SOURCE_DIR}/kig/kig/hi16-app-kig.png
    ${CMAKE_CURRENT_SOURCE_DIR}/kig/kig/hi32-app-kig.png
    ${CMAKE_CURRENT_SOURCE_DIR}/kig/kig/hi48-app-kig.png
    ${CMAKE_CURRENT_SOURCE_DIR}/kig/kig/hi64-app-kig.png
    ${CMAKE_CURRENT_SOURCE_DIR}/kig/kig/hi128-app-kig.png
)
kde4_add_win32_app_icon(kig kig_SRCS ${kig_icons})
kde4_add_executable(kig ${kig_SRCS})

target_link_libraries(kig ${KDE4_KPARTS_LIBS})

install(TARGETS kig DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES kig.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kig_part.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kigui.rc kigpartui.rc DESTINATION ${DATA_INSTALL_DIR}/kig )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

