INCLUDES=$(all_includes) $(PYTHONINC)
KDE_CXXFLAGS=$(USE_EXCEPTIONS)
noinst_LTLIBRARIES=libscripting.la
noinst_HEADERS = \
	python_type.h \
	python_scripter.h \
	script_mode.h \
	newscriptwizard.h
libscripting_la_SOURCES = \
	python_type.cc \
	python_scripter.cc \
	script_mode.cc \
	newscriptwizardbase.ui \
	newscriptwizard.cc
libscripting_la_LDFLAGS = $(PYTHONLIB)
libscripting_la_LIBADD = $(LIBPYTHON) -lboost_python
#METASOURCES=AUTO
