project(kig)

#
# taken from kdeedu/CMakeList.txt
# from here...
set(CMAKE_MODULE_PATH ${kig_SOURCE_DIR}/cmake )

# search packages used by KDE
find_package(KDE4 4.3.80 REQUIRED)
if(WIN32)
  find_package(KDEWIN32 REQUIRED)
  # detect oxygen icon dir at configure time based on KDEDIRS - there may be different package installation locations
  execute_process(COMMAND "${KDE4_KDECONFIG_EXECUTABLE}" --path icon OUTPUT_VARIABLE _dir ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  file(TO_CMAKE_PATH "${_dir}" __dir)
  find_path(KDE4_ICON_DIR oxygen PATHS
      ${__dir}
  )
  message(STATUS "using oxygen application icons from ${KDE4_ICON_DIR}")
else(WIN32)
    set (KDE4_ICON_DIR  ${CMAKE_INSTALL_PREFIX}/share/icons)
endif(WIN32)

include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(MacroBoolTo01)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories (${QDBUS_INCLUDE_DIRS}  ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

# ...to here

find_package(BoostPython)

kde4_no_enable_final(kig)

include(KigConfigureChecks.cmake)

add_subdirectory( doc )
add_subdirectory( icons )
add_subdirectory( kig )
add_subdirectory( mimetypes )
add_subdirectory( macros )
# reenable after porting kig's kkfile plugns to the strigi api
#add_subdirectory( kfile )
add_subdirectory( data )
add_subdirectory( pykig )
if(BOOST_PYTHON_FOUND)
  add_subdirectory( scripting )
endif(BOOST_PYTHON_FOUND)

include_directories( ${CMAKE_SOURCE_DIR}/kig/modes )
if(BOOST_PYTHON_FOUND)
  include_directories(${BOOST_PYTHON_INCLUDES})
endif(BOOST_PYTHON_FOUND)

# kigpart

set(kigpart_PART_SRCS
   objects/angle_type.cc
   objects/arc_type.cc
   objects/base_type.cc
   objects/bogus_imp.cc
   objects/centerofcurvature_type.cc
   objects/circle_imp.cc
   objects/circle_type.cc
   objects/common.cc
   objects/conic_imp.cc
   objects/conic_types.cc
   objects/cubic_imp.cc
   objects/cubic_type.cc
   objects/curve_imp.cc
   objects/intersection_types.cc
   objects/inversion_type.cc
   objects/line_imp.cc
   objects/line_type.cc
   objects/locus_imp.cc
   objects/object_calcer.cc
   objects/object_drawer.cc
   objects/object_factory.cc
   objects/object_holder.cc
   objects/object_imp.cc
   objects/object_imp_factory.cc
   objects/object_type.cc
   objects/object_type_factory.cc
   objects/other_imp.cc
   objects/other_type.cc
   objects/point_imp.cc
   objects/point_type.cc
   objects/polygon_imp.cc
   objects/polygon_type.cc
   objects/bezier_imp.cc
   objects/bezier_type.cc
   objects/special_imptypes.cc
   objects/tangent_type.cc
   objects/tests_type.cc
   objects/text_imp.cc
   objects/text_type.cc
   objects/transform_types.cc
   objects/vector_type.cc
   misc/argsparser.cpp
   misc/builtin_stuff.cc
   misc/calcpaths.cc
   misc/common.cpp
   misc/conic-common.cpp
   misc/coordinate.cpp
   misc/coordinate_system.cpp
   misc/cubic-common.cc
   misc/equation.cc
   misc/goniometry.cc
   misc/guiaction.cc
   misc/kigcoordinateprecisiondialog.cpp
   misc/kigfiledialog.cc
   misc/kiginputdialog.cc
   misc/kignumerics.cpp
   misc/kigpainter.cpp
   misc/kigtransform.cpp
   misc/lists.cc
   misc/object_constructor.cc
   misc/object_hierarchy.cc
   misc/rect.cc
   misc/screeninfo.cc
   misc/special_constructors.cc
   misc/unit.cc
   modes/base_mode.cc
   modes/construct_mode.cc
   modes/dragrectmode.cc
   modes/edittype.cc
   modes/historydialog.cc
   modes/label.cc
   modes/linkslabel.cpp
   modes/macro.cc
   modes/macrowizard.cc
   modes/mode.cc
   modes/moving.cc
   modes/normal.cc
   modes/popup.cc
   modes/textlabelwizard.cc
   modes/typesdialog.cpp
   filters/asyexporter.cc
   filters/asyexporteroptions.cc
   filters/asyexporterimpvisitor.cc
   filters/cabri-filter.cc
   filters/cabri-utils.cc
   filters/drgeo-filter.cc
   filters/exporter.cc
   filters/filter.cc
   filters/filters-common.cc
   filters/imageexporteroptions.cc
   filters/kgeo-filter.cc
   filters/kseg-filter.cc
   filters/latexexporter.cc
   filters/latexexporteroptions.cc
   filters/native-filter.cc
   filters/svgexporter.cc
   filters/svgexporteroptions.cc
   filters/xfigexporter.cc
   kig/kig_commands.cpp
   kig/kig_document.cc
   kig/kig_part.cpp
   kig/kig_view.cpp
)

kde4_add_ui_files(kigpart_PART_SRCS
   modes/typeswidget.ui
   modes/edittypewidget.ui
   modes/historywidget.ui
   filters/asyexporteroptionswidget.ui
   filters/imageexporteroptionswidget.ui
   filters/latexexporteroptionswidget.ui
   filters/svgexporteroptionswidget.ui
   misc/kigcoordinateprecisiondialog.ui
)

if(BOOST_PYTHON_FOUND)
  set(kigpart_PART_SRCS ${kigpart_PART_SRCS}
     scripting/newscriptwizard.cc
     scripting/python_scripter.cc
     scripting/python_type.cc
     scripting/script-common.cc
     scripting/script_mode.cc
  )

  set_source_files_properties(scripting/python_scripter.cc PROPERTIES COMPILE_FLAGS "${KDE4_ENABLE_EXCEPTIONS}")
endif(BOOST_PYTHON_FOUND)


kde4_add_plugin(kigpart ${kigpart_PART_SRCS})

target_link_libraries(kigpart ${KDE4_KPARTS_LIBS} ${KDE4_KUTILS_LIBS} )
if(BOOST_PYTHON_FOUND)
  target_link_libraries(kigpart ${BOOST_PYTHON_LIBS} ${KDE4_KTEXTEDITOR_LIBS})
endif(BOOST_PYTHON_FOUND)

install(TARGETS kigpart DESTINATION ${PLUGIN_INSTALL_DIR})


# unit tests
add_subdirectory(tests)

