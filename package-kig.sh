#! /bin/bash

# this is mostly a log for myself for remembering how to build the kig
# packages.  There are a lot of options to Jason Katz-Brown and
# Sebastain Stein's cvs2dist program (
# http://www.katzbrown.com/shiritsu/programming/cvs2dist/ ) and i
# wrote some scripts to remember what options i want..

OLDPWD=$(pwd)

VERSION="0.3"
NAME="kig"
I18NDIR="/home/domi/src/kde-i18n"
ADMINDIR="/home/domi/src/kde-common/admin"

TEMPDIR="/tmp/$NAME-$VERSION-package-temp"
rm -rf $TEMPDIR
mkdir $TEMPDIR
cd $TEMPDIR

cd $I18NDIR
I18NLANGS=$(find -name 'kig.po' | grep -v docs | sed -e 's#\./\([^/]*\)/.*#\1#')
echo $I18NLANGS

cd $TEMPDIR
~domi/downloads/cvs2dist \
	--name "$NAME" \
	--version "$VERSION" \
	--admin-dir="$ADMINDIR" \
	--i18n-dir "$I18NDIR" \
	--i18n-langs="$I18NLANGS" \
	--log="$TEMPDIR/log" \
	~/src/test/kdenonbeta kig

cd $OLDPWD

echo " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "
echo "Niet vergeten om #MIN_CONFIG in "
echo "configure.in.in te veranderen in "
echo "#MIN_CONFIG( 3.0 ) zodat kig niet afhangt "
echo "van Qt 3.1 maar Qt 3.0..."
