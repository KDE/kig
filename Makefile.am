SUBDIRS = objects misc modes icons filters examples kig mimetypes

messages: rc.cpp
	$(EXTRACTRC) *.rc */*.rc >> rc.cpp
	$(EXTRACTRC) $(find -name '*.ui') >> rc.cpp
	$(XGETTEXT) -ki18n -kI18N_NOOP rc.cpp */*.h */*.cc */*.cpp -o $(podir)/kig.pot
