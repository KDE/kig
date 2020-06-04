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
$XGETTEXT tips.cpp rc.cpp $(find . -name "*.cpp" -o -name "*.h" -o -name "*.cc") -o $podir/kig.pot
rm -f tips.cpp
