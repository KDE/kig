
kde4_add_unit_test(coordinatevalidatortest coordinatevalidatortest.cpp)
target_link_libraries(coordinatevalidatortest ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
