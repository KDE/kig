kde4_header()

install_files(${DATA_INSTALL_DIR}/kig/builtin-macros FILES
   circle_by_center_and_line.kigt
   circle_by_point_and_diameter.kigt
   equitriangle.kigt
   evolute.kigt
   osculating_circle.kigt
   segment_axis.kigt
   square.kigt
   vector_difference.kigt
)

kde4_footer()

#original Makefile.am contents follow:

#builtinmacrodir	= $(kde_datadir)/kig/builtin-macros
#builtinmacro_DATA = \
#	circle_by_center_and_line.kigt \
#	circle_by_point_and_diameter.kigt \
#	equitriangle.kigt \
#	evolute.kigt \
#	osculating_circle.kigt \
#	segment_axis.kigt \
#	square.kigt \
#	vector_difference.kigt
