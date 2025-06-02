cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "04")
set(RELEASE_SERVICE_VERSION_MICRO "2")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kig VERSION ${RELEASE_SERVICE_VERSION})


set (KDE5_ICON_DIR  ${CMAKE_INSTALL_PREFIX}/share/icons)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${kig_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.90.0")

find_package(KF5Parts ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5DocTools ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5TextEditor ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Archive ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5XmlGui ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Crash ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF5_MIN_VERSION} REQUIRED)
find_package(Qt${QT_MAJOR_VERSION}Svg ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt${QT_MAJOR_VERSION}PrintSupport ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt${QT_MAJOR_VERSION}XmlPatterns ${QT_REQUIRED_VERSION})


ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KIG VERSION_HEADER kig_version.h)

add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS -DQT_NO_KEYWORDS)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})


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
   Qt${QT_MAJOR_VERSION}XmlPatterns PROPERTIES
   DESCRIPTION "Qt${QT_MAJOR_VERSION} XmlPatterns Module"
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

   kig/kig_part.qrc


   objects/angle_type.h
   objects/arc_type.h
   objects/base_type.h
   objects/bogus_imp.h
   objects/centerofcurvature_type.h
   objects/circle_imp.h
   objects/circle_type.h
   objects/common.h
   objects/conic_imp.h
   objects/conic_types.h
   objects/cubic_imp.h
   objects/cubic_type.h
   objects/curve_imp.h
   objects/intersection_types.h
   objects/inversion_type.h
   objects/line_imp.h
   objects/line_type.h
   objects/locus_imp.h
   objects/object_calcer.h
   objects/object_drawer.h
   objects/object_factory.h
   objects/object_holder.h
   objects/object_imp.h
   objects/object_imp_factory.h
   objects/object_type.h
   objects/object_type_factory.h
   objects/other_imp.h
   objects/other_type.h
   objects/point_imp.h
   objects/point_type.h
   objects/polygon_imp.h
   objects/polygon_type.h
   objects/bezier_imp.h
   objects/bezier_type.h
   objects/special_imptypes.h
   objects/tangent_type.h
   objects/tests_type.h
   objects/text_imp.h
   objects/text_type.h
   objects/transform_types.h
   objects/vector_type.h
   misc/argsparser.h
   misc/builtin_stuff.h
   misc/calcpaths.h
   misc/common.h
   misc/conic-common.h
   misc/coordinate.h
   misc/coordinate_system.h
   misc/cubic-common.h
   misc/equationstring.h
   misc/goniometry.h
   misc/guiaction.h
   misc/kigcoordinateprecisiondialog.h
   misc/kigfiledialog.h
   misc/kiginputdialog.h
   misc/kignumerics.h
   misc/kigpainter.h
   misc/kigtransform.h
   misc/lists.h
   misc/object_constructor.h
   misc/object_hierarchy.h
   misc/rect.h
   misc/screeninfo.h
   misc/special_constructors.h
   misc/unit.h
   modes/base_mode.h
   modes/construct_mode.h
   modes/dragrectmode.h
   modes/edittype.h
   modes/historydialog.h
   modes/label.h
   modes/linkslabel.h
   modes/macro.h
   modes/macrowizard.h
   modes/mode.h
   modes/moving.h
   modes/normal.h
   modes/popup/popup.h
   modes/popup/objectchooserpopup.h
   modes/popup/propertiesactionsprovider.h
   modes/popup/objectchooserpopup.h
   modes/popup/objecttypeactionsprovider.h
   modes/popup/nameobjectactionsprovider.h
   modes/popup/objectconstructoractionsprovider.h
   modes/popup/builtindocumentactionsprovider.h
   modes/popup/builtinobjectactionsprovider.h
   modes/textlabelwizard.h
   modes/typesdialog.h
   filters/asyexporter.h
   filters/asyexporteroptions.h
   filters/asyexporterimpvisitor.h
   filters/cabri-filter.h
   filters/cabri-utils.h
   filters/drgeo-filter.h
   filters/exporter.h
   filters/filter.h
   filters/filters-common.h
   filters/imageexporteroptions.h
   filters/kgeo-filter.h
   filters/kseg-filter.h
   filters/latexexporter.h
   filters/latexexporteroptions.h
   filters/native-filter.h
   filters/pgfexporterimpvisitor.h
   filters/svgexporter.h
   filters/svgexporteroptions.h
   filters/xfigexporter.h
   kig/kig_commands.h
   kig/kig_document.h
   kig/kig_part.h
   kig/kig_view.h
)

if (Qt${QT_MAJOR_VERSION}XmlPatterns_FOUND)
  set(kigpart_PART_SRCS ${kigpart_PART_SRCS}
  geogebra/geogebratransformer.cpp
  geogebra/geogebrasection.cpp
  filters/geogebra-filter.cpp)
  qt_add_resources(kigpart_PART_SRCS ${CMAKE_SOURCE_DIR}/geogebra/geogebra.qrc)
  add_definitions(-DWITH_GEOGEBRA=1)
endif()

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

target_link_libraries(kigpart
  Qt::Gui
  Qt::Svg
  Qt::PrintSupport
  KF5::Crash
  KF5::Parts
  KF5::I18n
  KF5::TextEditor
  KF5::IconThemes
  KF5::ConfigWidgets
  KF5::Archive
)

if(BoostPython_FOUND)
  target_link_libraries(kigpart ${BoostPython_LIBRARIES} ${KDE5_KTEXTEDITOR_LIBS})
endif(BoostPython_FOUND)

if (Qt${QT_MAJOR_VERSION}XmlPatterns_FOUND)
  target_link_libraries(kigpart Qt::XmlPatterns)
endif(Qt${QT_MAJOR_VERSION}XmlPatterns_FOUND)

ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()
install(TARGETS kigpart DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/parts)
install(FILES org.kde.kig.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# unit tests
if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
