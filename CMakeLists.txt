project(kig)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM REQUIRED NO_MODULE)
find_package(KF5Parts REQUIRED)
find_package(KF5DocTools REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(KF5TextEditor REQUIRED)
find_package(KF5IconThemes REQUIRED)
find_package(KF5KDELibs4Support REQUIRED)

# Needed for unit tests
find_package(KF5Emoticons)
find_package(KF5ItemModels)

set(CMAKE_MODULE_PATH ${kig_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

# TODO Figure this out for Frameworks
# search packages used by KDE
if(WIN32)
  find_package(KDEWIN32 REQUIRED)
  # detect oxygen icon dir at configure time based on KDEDIRS - there may be different package installation locations
  execute_process(COMMAND "${KDE5_KDECONFIG_EXECUTABLE}" --path icon OUTPUT_VARIABLE _dir ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  file(TO_CMAKE_PATH "${_dir}" __dir)
  find_path(KDE5_ICON_DIR oxygen PATHS
      ${__dir}
  )
  message(STATUS "using oxygen application icons from ${KDE5_ICON_DIR}")
else(WIN32)
    set (KDE5_ICON_DIR  ${CMAKE_INSTALL_PREFIX}/share/icons)
endif(WIN32)

include(ECMInstallIcons)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)

add_definitions (${QT_DEFINITIONS} ${KDE5_DEFINITIONS} -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories (${Qt5Gui_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE5_INCLUDES})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(BoostPython)

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

if(BoostPython_FOUND)
  add_subdirectory( scripting )
  add_definitions(-DKIG_ENABLE_PYTHON_SCRIPTING)
endif(BoostPython_FOUND)

set_package_properties(
   FEATURE PROPERTIES
   DESCRIPTION "Boost.Python"
   URL "http://www.boost.org/"
   TYPE OPTIONAL
   PURPOSE "Kig can optionally use Boost.Python for Python scripting"
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

include_directories( ${CMAKE_SOURCE_DIR}/modes )
if(BoostPython_FOUND)
  include_directories(${BoostPython_INCLUDE_DIRS})
endif(BoostPython_FOUND)

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
   modes/popup/popup.cc
   modes/popup/objectchooserpopup.cc
   modes/popup/propertiesactionsprovider.cc
   modes/popup/objectchooserpopup.cc
   modes/popup/objecttypeactionsprovider.cc
   modes/popup/nameobjectactionsprovider.cc
   modes/popup/objectconstructoractionsprovider.cc
   modes/popup/builtindocumentactionsprovider.cc
   modes/popup/builtinobjectactionsprovider.cc
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
   filters/pgfexporterimpvisitor.cc
   filters/svgexporter.cc
   filters/svgexporteroptions.cc
   filters/xfigexporter.cc
   kig/kig_commands.cpp
   kig/kig_document.cc
   kig/kig_part.cpp
   kig/kig_view.cpp
)

qt5_wrap_ui(kigpart_PART_SRCS
   modes/typeswidget.ui
   modes/edittypewidget.ui
   modes/historywidget.ui
   filters/asyexporteroptionswidget.ui
   filters/imageexporteroptionswidget.ui
   filters/latexexporteroptionswidget.ui
   filters/svgexporteroptionswidget.ui
   misc/kigcoordinateprecisiondialog.ui
)

if(BoostPython_FOUND)
  set(kigpart_PART_SRCS ${kigpart_PART_SRCS}
     modes/popup/scriptactionsprovider.cc
     scripting/newscriptwizard.cc
     scripting/python_scripter.cc
     scripting/python_type.cc
     scripting/script-common.cc
     scripting/script_mode.cc
  )

  set_source_files_properties(scripting/python_scripter.cc PROPERTIES COMPILE_FLAGS "${KDE5_ENABLE_EXCEPTIONS}")
endif(BoostPython_FOUND)


add_library(kigpart MODULE ${kigpart_PART_SRCS})

target_link_libraries(kigpart KF5::Parts KF5::I18n KF5::TextEditor KF5::IconThemes Qt5::Gui ${KDE5_KUTILS_LIBS} )

if(BoostPython_FOUND)
  target_link_libraries(kigpart ${BoostPython_LIBRARIES} ${KDE5_KTEXTEDITOR_LIBS})
endif(BoostPython_FOUND)

install(TARGETS kigpart DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kig.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)

# unit tests
add_subdirectory(tests)
