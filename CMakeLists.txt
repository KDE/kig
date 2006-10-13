project(kig)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kig.h )


set(libkigparttemp_SRCS
   kig/kig_commands.cpp
   kig/kig_document.cc
   kig/kig_part.cpp
   kig/kig_view.cpp
)

set(libmodes_SRCS
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
)
kde4_add_ui3_files(libmodes_SRCS
   modes/macrowizardbase.ui
   modes/textlabelwizardbase.ui
)
kde4_add_ui_files(libmodes_SRCS
   modes/typeswidget.ui
   modes/edittypewidget.ui
   modes/historywidget.ui
)

set(libmisc_SRCS
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
)

set(libfilters_SRCS
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
)
kde4_add_ui_files(libfilters_SRCS
   filters/imageexporteroptionswidget.ui
   filters/latexexporteroptionswidget.ui
   filters/svgexporteroptionswidget.ui
)

set(libobjects_SRCS
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
   objects/tangent_type.cc
   objects/special_imptypes.cc
   objects/tests_type.cc
   objects/text_imp.cc
   objects/text_type.cc
   objects/transform_types.cc
   objects/vector_type.cc
)

if(BOOST_PYTHON_FOUND)
  set(libscripting_SRCS
     scripting/newscriptwizard.cc
     scripting/python_scripter.cc
     scripting/python_type.cc
     scripting/script-common.cc
     scripting/script_mode.cc
  )
  kde4_add_ui3_files(libscripting_SRCS
     scripting/newscriptwizardbase.ui
  )

  set_source_files_properties(scripting/python_scripter.cc PROPERTIES COMPILE_FLAGS "-fexceptions")
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

kde4_automoc(kigpart ${kigpart_PART_SRCS})

kde4_add_plugin(kigpart WITH_PREFIX ${kigpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kigpart )

target_link_libraries(kigpart  ${KDE4_KPARTS_LIBS} ${KDE4_KDEPRINT_LIBS} )
if(BOOST_PYTHON_FOUND)
  target_link_libraries(kigpart ${BOOST_PYTHON_LIBS} ktexteditor kdocument)
endif(BOOST_PYTHON_FOUND)

install(TARGETS kigpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############





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
