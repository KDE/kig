
EXTRA_DIST = splash.png thales.png traceExample.png 

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/kig/pics/
	$(INSTALL_DATA) $(srcdir)/splash.png $(kde_datadir)/kig/pics/splash.png
	$(mkinstalldirs) $(kde_datadir)/kig/pics/
	$(INSTALL_DATA) $(srcdir)/thales.png $(kde_datadir)/kig/pics/thales.png
	$(mkinstalldirs) $(kde_datadir)/kig/pics/
	$(INSTALL_DATA) $(srcdir)/traceExample.png $(kde_datadir)/kig/pics/traceExample.png

uninstall-local:
	-rm -f $(kde_datadir)/kig/pics/splash.png
	-rm -f $(kde_datadir)/kig/pics/thales.png
	-rm -f $(kde_datadir)/kig/pics/traceExample.png
