
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


# kig

set(kig_SRCS
  main.cpp
  kig.cpp
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-kig.png")
ecm_add_app_icon(kig_SRCS ICONS ${ICONS_SRCS})
add_executable(kig ${kig_SRCS})

target_link_libraries(kig KF5::Parts KF5::ConfigCore KF5::Crash)

install(TARGETS kig ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( PROGRAMS org.kde.kig.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES kig_part.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
install( FILES kigui.rc kigpartui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kig )

ecm_install_icons(ICONS
  22-apps-kig.png
  16-apps-kig.png
  32-apps-kig.png
  64-apps-kig.png
  48-apps-kig.png
  128-apps-kig.png
  sc-apps-kig.svgz
  DESTINATION ${KDE_INSTALL_ICONDIR}
  THEME hicolor
)


