
include_directories(  ${CMAKE_CURRENT_BINARY_DIR}/..  )


# kig

set(kig_SRCS
  main.cpp
  kig.cpp
)


if(MSVC)
  set(kig_SRCS ${kig_SRCS} kig.rc)
endif(MSVC)

if(MINGW)
   KDE_ADD_RESOURCES2(kig_SRCS kig.rc)
endif(MINGW)

kde4_add_executable(kig ${kig_SRCS})

target_link_libraries(kig ${KDE4_KPARTS_LIBS})

install(TARGETS kig DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES kig.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kig_part.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kigui.rc kigpartui.rc DESTINATION ${DATA_INSTALL_DIR}/kig )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

