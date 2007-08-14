/*
 Copyright (C) 2007      Pino Toscano <pino@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
*/

#include "../misc/common.h"

#include <qtest_kde.h>

#include <qobject.h>
#include <qvalidator.h>

// this class is copied from coordinate_system.cpp, with the following differences:
// - QDoubleValidator is used instead of KDoubleValidator
// this way, we avoid all the dependancies needed by the coordinate systems
class CoordinateValidator
  : public QValidator
{
  bool mpolar;
  QDoubleValidator mdv;
  mutable QRegExp mre;
public:
  CoordinateValidator( bool polar );
  ~CoordinateValidator();
  State validate ( QString & input,  int & pos ) const;
  void fixup ( QString & input ) const;
};


CoordinateValidator::CoordinateValidator( bool polar )
  : QValidator( 0L ), mpolar( polar ), mdv( 0L ),
    mre( polar ? "\\(? ?([0-9.,+-]+); ?([0-9.,+-]+) ?? ?\\)?"
         : "\\(? ?([0-9.,+-]+); ?([0-9.,+-]+) ?\\)?" )
{
}

CoordinateValidator::~CoordinateValidator()
{
}

static QString withoutSpaces( const QString& str )
{
  QString newstr;
  int s = str.size();
  for ( int i = 0; i < s; ++i )
    if ( !str.at( i ).isSpace() )
      newstr.append( str.at( i ) );
  return newstr;
}

QValidator::State CoordinateValidator::validate( QString & input, int & pos ) const
{
  QString tinput = withoutSpaces( input );
  if ( tinput.isEmpty() )
    return Invalid;
  if ( tinput.at( tinput.length() - 1 ) == ')' ) tinput.truncate( tinput.length() - 1 );
  if ( mpolar )
  {
    // strip the eventual '°'
    if ( !tinput.isEmpty() && tinput.at( tinput.length() - 1 ).unicode() == 176 )
      tinput.truncate( tinput.length() - 1 );
  };
  if ( tinput[0] == '(' ) tinput = tinput.mid( 1 );
  int scp = tinput.indexOf( ';' );
  if ( scp == -1 ) return mdv.validate( tinput, pos ) == Invalid ? Invalid : Intermediate;
  else
  {
    QString p1 = tinput.left( scp );
    QString p2 = tinput.mid( scp + 1 );

    State ret = Acceptable;

    int boguspos = 0;
    ret = kigMin( ret, mdv.validate( p1, boguspos ) );

    boguspos = 0;
    ret = kigMin( ret, mdv.validate( p2, boguspos ) );

    return ret;
  };
}

void CoordinateValidator::fixup( QString & input ) const
{
  int nsc = input.count( ';' );
  if ( nsc > 1 )
  {
    // where is the second ';'
    int i = input.indexOf( ';' );
    i = input.indexOf( ';', i );
    input = input.left( i );
  };
  // now the string has at most one semicolon left..
  int sc = input.indexOf( ';' );
  if ( sc == -1 )
  {
    sc = input.length();
    KLocale* l = KGlobal::locale();
    if ( mpolar )
      input.append( QString::fromLatin1( ";" ) + l->positiveSign() +
                    QString::fromLatin1( "0" ) );
    else
      input.append( QString::fromLatin1( ";" ) + l->positiveSign() +
                    QString::fromLatin1( "0" ) + l->decimalSymbol() +
                    QString::fromLatin1( "0" ) );
  };
  mre.exactMatch( input );
  QString ds1 = mre.cap( 1 );
  mdv.fixup( ds1 );
  QString ds2 = mre.cap( 2 );
  mdv.fixup( ds2 );
  input = ds1 + QString::fromLatin1( "; " ) + ds2;
}


class CoordinateValidatorTest
  : public QObject
{
  Q_OBJECT
public:
  CoordinateValidatorTest();

private slots:
  void testEuclideanSystem();
  void testPolarSystem();
};

CoordinateValidatorTest::CoordinateValidatorTest()
{
}

void CoordinateValidatorTest::testEuclideanSystem()
{
  CoordinateValidator v( false );

  QString s;
  int fakepos = 0;
  QCOMPARE( v.validate( s, fakepos ), QValidator::Invalid );
  s = QString::fromUtf8( "5" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( ")" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "()" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "(5.3;5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( "5.3;5" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( "  5.3 ;5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( ";5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "(;)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "4;5.6)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( ";5..6)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Invalid );
  s = QString::fromUtf8( "5;" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
}

void CoordinateValidatorTest::testPolarSystem()
{
  CoordinateValidator v( true );

  QString s;
  int fakepos = 0;
  QCOMPARE( v.validate( s, fakepos ), QValidator::Invalid );
  s = QString::fromUtf8( "°" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( ")" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "°)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "5" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "5°" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "(5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "(5.3;5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( "(5.3;5°)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( "5.3;5" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( "5.3;5°" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( "  5.3 ;5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( ";5)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( ";5°)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "(;)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "(;°)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
  s = QString::fromUtf8( "4;5.6)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Acceptable );
  s = QString::fromUtf8( ";5..6)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Invalid );
  s = QString::fromUtf8( "5..6;°)" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Invalid );
  s = QString::fromUtf8( "5;" );
  QCOMPARE( v.validate( s, fakepos ), QValidator::Intermediate );
}

QTEST_KDEMAIN_CORE( CoordinateValidatorTest )

#include "coordinatevalidatortest.moc"
