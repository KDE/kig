
cmake_minimum_required(VERSION 3.5)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "21")
set(RELEASE_SERVICE_VERSION_MINOR "08")
set(RELEASE_SERVICE_VERSION_MICRO "0")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kig VERSION ${RELEASE_SERVICE_VERSION})

find_package(Qt5Svg REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5XmlPatterns 5.5.1)

find_package(ECM 1.7 REQUIRED NO_MODULE)
find_package(KF5Parts REQUIRED)
find_package(KF5DocTools REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(KF5TextEditor REQUIRED)
find_package(KF5IconThemes REQUIRED)
find_package(KF5ConfigWidgets REQUIRED)
find_package(KF5Archive REQUIRED)
find_package(KF5XmlGui 5.1 REQUIRED)
find_package(KF5Crash 5.15 REQUIRED)
find_package(KF5CoreAddons REQUIRED)

set(CMAKE_MODULE_PATH ${kig_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

set (KDE5_ICON_DIR  ${CMAKE_INSTALL_PREFIX}/share/icons)

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KIG VERSION_HEADER kig_version.h)

add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories (${Qt5Gui_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE5_INCLUDES})


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include(KigConfigureChecks.cmake)

find_package(BoostPython)

add_subdirectory( doc )
add_subdirectory( icons )
add_subdirectory( kig )
add_subdirectory( mimetypes )
add_subdirectory( macros )
add_subdirectory( data )
add_subdirectory( pykig )

if(BoostPython_FOUND)
  add_subdirectory( scripting )
  add_definitions(-DKIG_ENABLE_PYTHON_SCRIPTING)
endif(BoostPython_FOUND)

set_package_properties(
   BoostPython PROPERTIES
   DESCRIPTION "Python scripting support in Boost"
   URL "https://www.boost.org/"
   TYPE OPTIONAL
   PURPOSE "Kig can optionally use Boost.Python for Python scripting"
)

set_package_properties(
   Qt5XmlPatterns PROPERTIES
   DESCRIPTION "Qt5 XmlPatterns Module"
   URL "https://qt.io/"
   TYPE OPTIONAL
   PURPOSE "Kig uses the XML Patterns module in Qt to support Geogebra files"
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
   misc/equationstring.cc
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

if (Qt5XmlPatterns_FOUND)
  set(kigpart_PART_SRCS ${kigpart_PART_SRCS}
  geogebra/geogebratransformer.cpp
  geogebra/geogebrasection.cpp
  filters/geogebra-filter.cpp)
  qt5_add_resources(kigpart_PART_SRCS ${CMAKE_SOURCE_DIR}/geogebra/geogebra.qrc)
  add_definitions(-DWITH_GEOGEBRA=1)
endif(Qt5XmlPatterns_FOUND)

ki18n_wrap_ui(kigpart_PART_SRCS
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

  kde_source_files_enable_exceptions(scripting/python_scripter.cc)
endif(BoostPython_FOUND)


add_library(kigpart MODULE ${kigpart_PART_SRCS})
generate_export_header(kigpart)

kcoreaddons_desktop_to_json(kigpart kig/kig_part.desktop)

target_link_libraries(kigpart
  Qt5::Gui
  Qt5::Svg
  Qt5::PrintSupport
  KF5::Crash
  KF5::Parts
  KF5::I18n
  KF5::TextEditor
  KF5::IconThemes
  KF5::ConfigWidgets
  KF5::Archive
  ${KDE5_KUTILS_LIBS}
)

if(BoostPython_FOUND)
  target_link_libraries(kigpart ${BoostPython_LIBRARIES} ${KDE5_KTEXTEDITOR_LIBS})
endif(BoostPython_FOUND)

if (Qt5XmlPatterns_FOUND)
  target_link_libraries(kigpart Qt5::XmlPatterns)
endif(Qt5XmlPatterns_FOUND)

ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()
install(TARGETS kigpart DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/parts)
install(FILES org.kde.kig.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# unit tests
if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()
