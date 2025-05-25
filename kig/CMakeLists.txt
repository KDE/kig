
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


# kig

set(kig_SRCS
  main.cpp
  kig.cpp
  kig.h
  kig.qrc
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-kig.png")
ecm_add_app_icon(kig_SRCS ICONS ${ICONS_SRCS})
add_executable(kig ${kig_SRCS})

target_link_libraries(kig KF6::Parts KF6::ConfigCore KF6::Crash KF6::I18n)

install(TARGETS kig ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(APPLE)
    set_target_properties(kig PROPERTIES
        MACOSX_BUNDLE_DISPLAY_NAME "Kig"
        MACOSX_BUNDLE_BUNDLE_NAME "Kig"
        MACOSX_BUNDLE_LONG_VERSION_STRING "Kig ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.kig"
        MACOSX_BUNDLE_COPYRIGHT "2002-2025 The Kig Developers")

endif()

########### install files ###############

install( PROGRAMS org.kde.kig.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )

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


