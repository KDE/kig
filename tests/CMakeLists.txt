set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

find_package(Qt5Test REQUIRED)

ecm_add_test(
   coordinatevalidatortest.cpp
   LINK_LIBRARIES ${KDE4_KDECORE_LIBS} Qt5::Gui Qt5::Test
)
