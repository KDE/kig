ksapidir=$(DESTDIR)$(kde_htmldir)/en/kig/scripting-api
ksapi_DATA = \
	bottom1.png \
	bottom2.png \
	docheadergears.png \
	grad.png \
	headerbg.png

# generate the Kig Python Scriptin APIdocs from the Kig source code
all-local:
	cat Doxyfile.scripting-api.in | sed -e 's#KIGVERSION#@KIGVERSION@#g' | sed -e 's#TOPSRCDIR#$(top_srcdir)#g' | sed -e 's#SRCDIR#$(srcdir)#g' > Doxyfile.scripting-api
	doxygen $(srcdir)/Doxyfile.scripting-api

install-data-local: install-kig-python-scripting-apidocs

install-kig-python-scripting-apidocs:
	kpsapidir="$(DESTDIR)$(kde_htmldir)/en/kig/scripting-api"; \
	$(mkinstalldirs) "$$kpsapidir"; \
	list=`ls ./build/html`; \
	for file in $$list; do \
		echo $(INSTALL_DATA) ./build/html/$$file $$kpsapidir; \
		$(INSTALL_DATA) ./build/html/$$file $$kpsapidir; \
	done

uninstall-local: uninstall-kig-python-scripting-apidocs

uninstall-kig-python-scripting-apidocs:
	kpsapidir="$(DESTDIR)$(kde_htmldir)/en/kig/scripting-api"; \
	if test -d "$$kpsapidir"; then \
		rm -rfv "$$kpsapidir"; \
	fi
