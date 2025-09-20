



########### install files ###############





#original Makefile.am contents follow:

## generate the Kig Python Scripting APIdocs from the Kig source code
#
#all-local: build-kig-python-scripting-apidocs
#build-kig-python-scripting-apidocs:
##	cat $(srcdir)/Doxyfile.scripting-api.in | sed -e 's#KIGVERSION#@KIGVERSION@#g' | sed -e 's#TOPSRCDIR#$(top_srcdir)#g' | sed -e 's#SRCDIR#$(srcdir)#g' > Doxyfile.scripting-api
#	doxygen Doxyfile.scripting-api
