kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files(  FILES cubic-locus.kig  	ellipse.kig  	figure_angle.fgeo  	figure_manyobjects.fgeo  	fregier.kigt  	locustest.kig  	parabolaBDF.kigt  	session_alotofthings.fgeo  	sine-curve.kig  	sine-curve.png )

kde4_footer()



#original Makefile.am contents follow:

#noinst_DATA= \
#	cubic-locus.kig \
#	ellipse.kig \
#	figure_angle.fgeo \
#	figure_manyobjects.fgeo \
#	fregier.kigt \
#	locustest.kig \
#	parabolaBDF.kigt \
#	session_alotofthings.fgeo \
#	sine-curve.kig \
#	sine-curve.png
