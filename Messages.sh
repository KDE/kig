#! /bin/sh
$EXTRACTRC */*.rc >> rc.cpp || exit 11
$EXTRACTRC */*.ui >> rc.cpp || exit 12
(cd data && $PREPARETIPS > ../tips.cpp)
for file in macros/*.kigt; do
	cat "$file" | grep '<Name>' | sed -e 's/^ *<Name>\([^<]*\)<\/Name>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp
	cat "$file" | grep '<Description>' | sed -e 's/^ *<Description>\([^<]*\)<\/Description>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp
	cat "$file" | grep '<UseText>' | sed -e 's/^ *<UseText>\([^<]*\)<\/UseText>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp
	cat "$file" | grep '<SelectStatement>' | sed -e 's/^ *<SelectStatement>\([^<]*\)<\/SelectStatement>/i18n( "\1" );/' | sed -e 's/&amp;/\&/g' >> rc.cpp
done
$XGETTEXT tips.cpp rc.cpp filters/*.h kig/*.h misc/*.h modes/*.h objects/*.h scripting/*.h */*.cc kig/*.cpp misc/*.cpp modes/*.cpp -o $podir/kig.pot
$XGETTEXT kfile/kfile_drgeo.cpp kfile/kfile_drgeo.h -o $podir/kfile_drgeo.pot
$XGETTEXT kfile/kfile_kig.cpp kfile/kfile_kig.h -o $podir/kfile_kig.pot
rm -f tips.cpp
