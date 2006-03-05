kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

## generate the Kig Python Scriptin APIdocs from the Kig source code
#
#all-local: build-kig-python-scripting-apidocs
#build-kig-python-scripting-apidocs:
##	cat $(srcdir)/Doxyfile.scripting-api.in | sed -e 's#KIGVERSION#@KIGVERSION@#g' | sed -e 's#TOPSRCDIR#$(top_srcdir)#g' | sed -e 's#SRCDIR#$(srcdir)#g' > Doxyfile.scripting-api
#	doxygen Doxyfile.scripting-api
#	cp bottom1.png bottom2.png docheadergears.png grad.png headerbg.png build/html
