tempfile=`tempfile`
cat kig/aboutdata.h | sed -ne '/tmp->addAuthor/,/setTranslator/p' | sed -e '$d' > $tempfile
tempfile2=`tempfile`
cat $tempfile | tr $'\n' " " | sed -e 's/"[[:blank:]]*"//g' | sed -e 's/I18N_NOOP([[:blank:]]*\("[^"]*"\)[[:blank:]]*)/\1/g' | sed -e 's/,[[:blank:]]*"/, "/g' | sed -e 's/^[[:blank:]]*//g' | sed -e 's/;[[:blank:]]*/\n/g' > $tempfile2
cat $tempfile2 | grep addAuthor | sed -e 's/tmp->addAuthor(\([^,]*\), "[^"]*",\([^)]*\))/  $appinfo->addAuthor(\1,\2);/g' | sed -e 's/@/ AT /g' | sed -e 's/\./ DOT /g'
cat $tempfile2 | sed -e 's/[^(]*(\([^,]*\), \("[^"]*"\), \([^)]*\))/  $appinfo->addContributor( \1, \3, \2 );/g'

rm $tempfile
rm $tempfile2
