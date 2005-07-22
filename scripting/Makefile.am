INCLUDES=$(all_includes) $(BOOST_PYTHON_INCLUDES)
KDE_CXXFLAGS=$(USE_EXCEPTIONS)
noinst_LTLIBRARIES=libscripting.la
noinst_HEADERS = \
	python_type.h \
	python_scripter.h \
	script-common.h \
	script_mode.h \
	newscriptwizard.h
libscripting_la_SOURCES = \
	python_type.cc \
	python_scripter.cc \
	script-common.cc \
	script_mode.cc \
	newscriptwizardbase.ui \
	newscriptwizard.cc
libscripting_la_LIBADD = $(BOOST_PYTHON_LIBS) $(PYTHON_LIBS) -lkatepartinterfaces
libscripting_la_LDFLAGS = $(all_libraries) $(PYTHON_LDFLAGS)
METASOURCES=AUTO

xml_DATA = python-kig.xml
xmldir = $(kde_datadir)/katepart/syntax
