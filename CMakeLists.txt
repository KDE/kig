kde4_header()
project(kig)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kig.h )


set(libkigparttemp_SRCS
   ${CMAKE_SOURCE_DIR}/kig/kig/kig_commands.cpp
   ${CMAKE_SOURCE_DIR}/kig/kig/kig_document.cc
   ${CMAKE_SOURCE_DIR}/kig/kig/kig_part.cpp
   ${CMAKE_SOURCE_DIR}/kig/kig/kig_view.cpp
)

set(libmodes_SRCS
   ${CMAKE_SOURCE_DIR}/kig/modes/base_mode.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/construct_mode.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/dragrectmode.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/edittype.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/label.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/linkslabel.cpp
   ${CMAKE_SOURCE_DIR}/kig/modes/macro.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/macrowizard.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/mode.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/moving.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/normal.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/popup.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/textlabelwizard.cc
   ${CMAKE_SOURCE_DIR}/kig/modes/typesdialog.cpp
)
kde4_add_ui3_files(libmodes_SRCS
   ${CMAKE_SOURCE_DIR}/kig/modes/macrowizardbase.ui
   ${CMAKE_SOURCE_DIR}/kig/modes/textlabelwizardbase.ui
)
kde4_add_ui_files(libmodes_SRCS
   ${CMAKE_SOURCE_DIR}/kig/modes/typeswidget.ui
   ${CMAKE_SOURCE_DIR}/kig/modes/edittypewidget.ui
)

set(libmisc_SRCS
   ${CMAKE_SOURCE_DIR}/kig/misc/argsparser.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/builtin_stuff.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/calcpaths.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/common.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/conic-common.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/coordinate.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/coordinate_system.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/cubic-common.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/equation.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/goniometry.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/guiaction.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/kigfiledialog.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/kiginputdialog.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/kignumerics.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/kigpainter.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/kigtransform.cpp
   ${CMAKE_SOURCE_DIR}/kig/misc/lists.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/object_constructor.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/object_hierarchy.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/rect.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/screeninfo.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/special_constructors.cc
   ${CMAKE_SOURCE_DIR}/kig/misc/unit.cc
)

set(libfilters_SRCS
   ${CMAKE_SOURCE_DIR}/kig/filters/cabri-filter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/cabri-utils.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/drgeo-filter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/exporter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/filter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/filters-common.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/imageexporteroptions.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/kgeo-filter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/kseg-filter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/latexexporter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/latexexporteroptions.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/native-filter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/svgexporter.cc
   ${CMAKE_SOURCE_DIR}/kig/filters/svgexporteroptions.cc
)
kde4_add_ui_files(libfilters_SRCS
   ${CMAKE_SOURCE_DIR}/kig/filters/imageexporteroptionswidget.ui
   ${CMAKE_SOURCE_DIR}/kig/filters/latexexporteroptionswidget.ui
   ${CMAKE_SOURCE_DIR}/kig/filters/svgexporteroptionswidget.ui
)

set(libobjects_SRCS
   ${CMAKE_SOURCE_DIR}/kig/objects/angle_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/arc_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/base_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/bogus_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/centerofcurvature_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/circle_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/circle_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/common.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/conic_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/conic_types.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/cubic_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/cubic_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/curve_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/intersection_types.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/inversion_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/line_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/line_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/locus_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_calcer.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_drawer.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_factory.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_holder.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_imp_factory.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/object_type_factory.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/other_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/other_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/point_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/point_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/polygon_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/polygon_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/tangent_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/special_imptypes.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/tests_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/text_imp.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/text_type.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/transform_types.cc
   ${CMAKE_SOURCE_DIR}/kig/objects/vector_type.cc
)

if(BOOST_PYTHON_FOUND)
  set(libscripting_SRCS
     ${CMAKE_SOURCE_DIR}/kig/scripting/newscriptwizard.cc
     ${CMAKE_SOURCE_DIR}/kig/scripting/python_scripter.cc
     ${CMAKE_SOURCE_DIR}/kig/scripting/python_type.cc
     ${CMAKE_SOURCE_DIR}/kig/scripting/script-common.cc
     ${CMAKE_SOURCE_DIR}/kig/scripting/script_mode.cc
  )
  kde4_add_ui3_files(libscripting_SRCS
     ${CMAKE_SOURCE_DIR}/kig/scripting/newscriptwizardbase.ui
  )

  set_source_files_properties(${CMAKE_SOURCE_DIR}/kig/scripting/python_scripter.cc PROPERTIES COMPILE_FLAGS "-fexceptions")
endif(BOOST_PYTHON_FOUND)

add_subdirectory( scripting )
add_subdirectory( icons )
add_subdirectory( kig )
add_subdirectory( mimetypes )
add_subdirectory( macros )
add_subdirectory( kfile )
add_subdirectory( data )
add_subdirectory( pykig )

include_directories( ${CMAKE_SOURCE_DIR}/kig/modes )

if(BOOST_PYTHON_FOUND)
   include_directories(${BOOST_PYTHON_INCLUDES})
endif(BOOST_PYTHON_FOUND)


########### next target ###############

set(kigpart_PART_SRCS
   ${libobjects_SRCS}
   ${libmisc_SRCS}
   ${libmodes_SRCS}
   ${libfilters_SRCS}
   ${libkigparttemp_SRCS}
   ${libscripting_SRCS}
)

kde4_automoc(${kigpart_PART_SRCS})

kde4_add_plugin(kigpart WITH_PREFIX ${kigpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kigpart )

target_link_libraries(kigpart  ${KDE4_KPARTS_LIBS} ${KDE4_KDEPRINT_LIBS} )
if(BOOST_PYTHON_FOUND)
  target_link_libraries(kigpart ${BOOST_PYTHON_LIBS} ktexteditor kdocument)
endif(BOOST_PYTHON_FOUND)

install_targets(${PLUGIN_INSTALL_DIR} kigpart )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#if KIG_COMPILE_PYTHON_SCRIPTING
#scriptingdir = scripting
#scriptinglib = scripting/libscripting.la
#else
#scriptingdir =
#scriptinglib =
#endif
#
#SUBDIRS = \
#	objects \
#	misc \
#	modes \
#	icons \
#	filters \
#	examples \
#	kig \
#	mimetypes \
#	macros \
#	kfile \
#	data \
#	pykig \
#	$(scriptingdir)
#
#kde_module_LTLIBRARIES = libkigpart.la
#libkigpart_la_SOURCES = dummy.cpp
#libkigpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#libkigpart_la_LIBADD  = $(LIB_KPARTS) \
#	misc/libmisc.la objects/libobjects.la filters/libfilters.la \
#	modes/libmodes.la kig/libkigparttemp.la $(scriptinglib)
#
#messages: rc.cpp
#	rm -f tips.cpp
#	$(EXTRACTRC) */*.rc >> rc.cpp
#	$(EXTRACTRC) */*.ui >> rc.cpp
#	(cd data && $(PREPARETIPS) > ../tips.cpp)
#	for file in macros/*.kigt; do \
#		cat "$$file" | grep '<Name>' | sed -e 's/^ *<Name>\([^<]*\)<\/Name>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
#		cat "$$file" | grep '<Description>' | sed -e 's/^ *<Description>\([^<]*\)<\/Description>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
#		cat "$$file" | grep '<UseText>' | sed -e 's/^ *<UseText>\([^<]*\)<\/UseText>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
#		cat "$$file" | grep '<SelectStatement>' | sed -e 's/^ *<SelectStatement>\([^<]*\)<\/SelectStatement>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp; \
#	done
#	$(XGETTEXT) tips.cpp rc.cpp filters/*.h kig/*.h misc/*.h modes/*.h objects/*.h scripting/*.h */*.cc kig/*.cpp misc/*.cpp modes/*.cpp -o $(podir)/kig.pot
#	$(XGETTEXT) kfile/kfile_drgeo.cpp kfile/kfile_drgeo.h -o $(podir)/kfile_drgeo.pot
#	$(XGETTEXT) kfile/kfile_kig.cpp kfile/kfile_kig.h -o $(podir)/kfile_kig.pot
#
#CLEANFILES = dummy.cpp
#
#dummy.cpp: 
#	touch dummy.cpp
#
#TAGS: 
#	find -name '*.cc' -o -name '*.cpp' -o -name '*.h' | etags -
#
## Want to do the scripting part separately, but
## include everything else.
#DOXYGEN_EXCLUDE = python-scripting-api-dox-mainpage.dox
#DOXYGEN_SET_RECURSIVE = YES
#include ../admin/Doxyfile.am
