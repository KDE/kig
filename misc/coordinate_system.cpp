/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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
**/

#include "coordinate_system.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include "common.h"
#include "coordinate.h"
#include "goniometry.h"
#include "kigpainter.h"

#include <qpainter.h>
#include <qregexp.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <knumvalidator.h>

#include <string>
#include <math.h>

class CoordinateValidator
  : public QValidator
{
  bool mpolar;
#ifdef KIG_USE_KDOUBLEVALIDATOR
  KDoubleValidator mdv;
#else
  KFloatValidator mdv;
#endif
  mutable QRegExp mre;
public:
  CoordinateValidator( bool polar );
  ~CoordinateValidator();
  State validate ( QString & input,  int & pos ) const;
  void fixup ( QString & input ) const;
};


CoordinateValidator::CoordinateValidator( bool polar )
  : QValidator( 0, 0 ), mpolar( polar ), mdv( 0, 0 ),
    mre( polar ? "\\(? ?([0-9.,+-]+); ?([0-9.,+-]+) ?�? ?\\)?"
         : "\\(? ?([0-9.,+-]+); ?([0-9.,+-]+) ?\\)?" )
{
}

CoordinateValidator::~CoordinateValidator()
{
}

QValidator::State CoordinateValidator::validate( QString & input, int & pos ) const
{
  QString tinput = input;
  if ( tinput[tinput.length() - 1 ] == ')' ) tinput.truncate( tinput.length() - 1 );
  if ( mpolar )
  {
    if ( tinput[tinput.length() - 1 ] == ' ' ) tinput.truncate( tinput.length() - 1 );
    if ( tinput[tinput.length() - 1 ] == '�' ) tinput.truncate( tinput.length() - 1 );
  };
  if( tinput[tinput.length() - 1 ] == ' ' ) tinput.truncate( tinput.length() - 1 );
  if ( tinput[0] == '(' ) tinput = tinput.mid( 1 );
  if( tinput[0] == ' ' ) tinput = tinput.mid( 1 );
  int scp = tinput.find( ';' );
  if ( scp == -1 ) return mdv.validate( tinput, pos ) == Invalid ? Invalid : Valid;
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
  int nsc = input.contains( ';' );
  if ( nsc > 1 )
  {
    // where is the second ';'
    int i = input.find( ';' );
    i = input.find( ';', i );
    input = input.left( i );
  };
  // now the string has at most one semicolon left..
  int sc = input.find( ';' );
  if ( sc == -1 )
  {
    sc = input.length();
    KLocale* l = KGlobal::locale();
    if ( mpolar )
      input.append( QString::fromLatin1( ";" ) + l->positiveSign() +
                    QString::fromLatin1( "0�" ) );
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

EuclideanCoords::EuclideanCoords()
{
}

QString EuclideanCoords::fromScreen( const Coordinate& p, const KigDocument& d ) const
{
  // i used to use the widget size here, but that's no good idea,
  // since an object isn't asked to recalc every time the widget size
  // changes..  might be a good idea to do that, but well, maybe some
  // other time :)
  Rect sr = d.suggestedRect();
  double m = kigMax( sr.width(), sr.height() );
  int l = kigMax( 0, (int) ( 3 - log10( m ) ) );
  QString xs = KGlobal::locale()->formatNumber( p.x, l );
  QString ys = KGlobal::locale()->formatNumber( p.y, l );
  return QString::fromLatin1( "( %1; %2 )" ).arg( xs ).arg( ys );
}

Coordinate EuclideanCoords::toScreen(const QString& s, bool& ok) const
{
  QRegExp r( "\\(? ?([0-9.,+-]+); ?([0-9.,+-]+) ?\\)?" );
  ok = ( r.search(s) == 0 );
  if (ok)
  {
    QString xs = r.cap(1);
    QString ys = r.cap(2);
    KLocale* l = KGlobal::locale();
    double x = l->readNumber( xs, &ok );
    if ( ! ok ) x = xs.toDouble( &ok );
    if ( ! ok ) return Coordinate();
    double y = l->readNumber( ys, &ok );
    if ( ! ok ) y = ys.toDouble( &ok );
    if ( ! ok ) return Coordinate();
    return Coordinate( x, y );
  }
  return Coordinate();
}

/**
 * copied and adapted from a ( public domain ) function i found in the
 * first Graphics Gems book.  Credits to Paul S. Heckbert, who wrote
 * the "Nice number for graph labels" gem.
 * find a "nice" number approximately equal to x.  We look for
 * 1, 2 or 5, multiplied by a power of 10.
 */
static double nicenum( double x, bool round )
{
  int exp = (int) log10( x );
  double f = x/pow( 10., exp );
  double nf;
  if ( round )
  {
    if ( f < 1.5 ) nf = 1.;
    else if ( f < 3. ) nf = 2.;
    else if ( f < 7. ) nf = 5.;
    else nf = 10.;
  }
  else
  {
    if ( f <= 1. ) nf = 1.;
    else if ( f <= 2. ) nf = 2.;
    else if ( f <= 5. ) nf = 5.;
    else nf = 10.;
  };
  return nf * pow( 10., exp );
}

void EuclideanCoords::drawGrid( KigPainter& p, bool showgrid, bool showaxes ) const
{
  p.setWholeWinOverlay();

  // this instruction in not necessary, but there is a little
  // optimization when there are no grid and no axes.
  if ( !( showgrid || showaxes ) )
    return;

  // this function is inspired upon ( public domain ) code from the
  // first Graphics Gems book.  Credits to Paul S. Heckbert, who wrote
  // the "Nice number for graph labels" gem.

  const double hmax = ceil( p.window().right() );
  const double hmin = floor( p.window().left() );
  const double vmax = ceil( p.window().top() );
  const double vmin = floor( p.window().bottom() );

  // the number of intervals we would like to have:
  // we try to have one of them per 40 pixels or so..
  const int ntick = static_cast<int>(
    kigMax( hmax - hmin, vmax - vmin ) / p.pixelWidth() / 40. ) + 1;

  double hrange = nicenum( hmax - hmin, false );
  double vrange = nicenum( vmax - vmin, false );
  const double newrange = kigMin( hrange, vrange );
  hrange = newrange;
  vrange = newrange;

  const double hd = nicenum( hrange / ( ntick - 1 ), true );
  const double vd = nicenum( vrange / ( ntick - 1 ), true );

  const double hgraphmin = ceil( hmin / hd) * hd;
  const double hgraphmax = floor( hmax / hd ) * hd;
  const double vgraphmin = ceil( vmin / vd ) * vd;
  const double vgraphmax = floor( vmax / vd ) * vd;

  const int hnfrac = kigMax( (int) - floor( log10( hd ) ), 0 );
  const int vnfrac = kigMax( (int) - floor( log10( vd ) ), 0 );

  /****** the grid lines ******/
  if ( showgrid )
  {
    p.setPen( QPen( lightGray, 0, DotLine ) );
    // vertical lines...
    for ( double i = hgraphmin; i <= hgraphmax + hd/2; i += hd )
      p.drawSegment( Coordinate( i, vgraphmin ),
                     Coordinate( i, vgraphmax ) );
    // horizontal lines...
    for ( double i = vgraphmin; i <= vgraphmax + vd/2; i += vd )
      p.drawSegment( Coordinate( hgraphmin, i ),
                     Coordinate( hgraphmax, i ) );
  }

  /****** the axes ******/
  if ( showaxes )
  {
    p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
    // x axis
    p.drawSegment( Coordinate( hmin, 0 ), Coordinate( hmax, 0 ) );
    // y axis
    p.drawSegment( Coordinate( 0, vmin ), Coordinate( 0, vmax ) );

    /****** the numbers ******/

    // x axis
    for( double i = hgraphmin; i <= hgraphmax + hd / 2; i += hd )
    {
      // we skip 0 since that would look stupid... (the axes going
      // through the 0 etc. )
      if( fabs( i ) < 1e-8 ) continue;

      p.drawText(
        Rect( Coordinate( i, 0 ), hd, -2*vd ).normalized(),
        KGlobal::locale()->formatNumber( i, hnfrac ),
        AlignLeft | AlignTop
        );
    };
    // y axis...
    for ( double i = vgraphmin; i <= vgraphmax + vd/2; i += vd )
    {
      if( fabs( i ) < 1e-8 ) continue;
      p.drawText ( Rect( Coordinate( 0, i ), 2*hd, vd ).normalized(),
                   KGlobal::locale()->formatNumber( i, vnfrac ),
                   AlignBottom | AlignLeft
        );
    };
    // arrows on the ends of the axes...
    p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
    p.setBrush( QBrush( Qt::gray ) );
    std::vector<Coordinate> a;

    // the arrow on the right end of the X axis...
    a.reserve( 3 );
    double u = p.pixelWidth();
    a.push_back( Coordinate( hmax - 6 * u, -3 * u) );
    a.push_back( Coordinate( hmax, 0 ) );
    a.push_back( Coordinate( hmax - 6 * u, 3 * u ) );
    p.drawArea( a );
//    p.drawPolygon( a, true );

    // the arrow on the top end of the Y axis...
    a.clear();
    a.reserve( 3 );
    a.push_back( Coordinate( 3 * u, vmax - 6 * u ) );
    a.push_back( Coordinate( 0, vmax ) );
    a.push_back( Coordinate( -3 * u, vmax - 6 * u ) );
    p.drawArea( a );
//    p.drawPolygon( a, true );
  }; // if( showaxes )
}

QString EuclideanCoords::coordinateFormatNotice() const
{
  return i18n( "Enter coordinates in the following format: \"x;y\",\n"
               "where x is the x coordinate, and y is the y coordinate." );
}

QString EuclideanCoords::coordinateFormatNoticeMarkup() const
{
  return i18n( "Enter coordinates in the following format: <b>\"x;y\"</b>, "
               "where x is the x coordinate, and y is the y coordinate." );
}

EuclideanCoords::~EuclideanCoords()
{
}

CoordinateSystem::~CoordinateSystem()
{
}

CoordinateSystem::CoordinateSystem()
{
}

PolarCoords::PolarCoords()
{
}

PolarCoords::~PolarCoords()
{
}

QString PolarCoords::fromScreen( const Coordinate& pt, const KigDocument& d ) const
{
  Rect sr = d.suggestedRect();
  double m = kigMax( sr.width(), sr.height() );
  int l = kigMax( 0, (int) ( 3 - log10( m ) ) );

  double r = pt.length();
  double theta = Goniometry::convert( atan2( pt.y, pt.x ), Goniometry::Rad, Goniometry::Deg );

  QString rs = KGlobal::locale()->formatNumber( r, l );
  QString ts = KGlobal::locale()->formatNumber( theta, 0 );

  return QString::fromLatin1("( %1; %2� )").arg( rs ).arg( ts );
}

QString PolarCoords::coordinateFormatNotice() const
{
  // \xCE\xB8 is utf8 for the greek theta sign..
  return i18n( "Enter coordinates in the following format: \"r; \xCE\xB8°\",\n"
               "where r and \xCE\xB8 are the polar coordinates." );
}

QString PolarCoords::coordinateFormatNoticeMarkup() const
{
  // \xCE\xB8 is utf8 for the greek theta sign..
  return i18n( "Enter coordinates in the following format: <b>\"r; \xCE\xB8°\"</b>, "
               "where r and \xCE\xB8 are the polar coordinates." );
}

Coordinate PolarCoords::toScreen(const QString& s, bool& ok) const
{
  QRegExp regexp("\\(? ?([0-9.,+-]+); ?([0-9.,+-]+) ?�? ?\\)?" );
  ok = ( regexp.search( s ) == 0 );
  if (ok)
  {
    QString rs = regexp.cap( 1 );
    double r = KGlobal::locale()->readNumber( rs, &ok );
    if ( ! ok ) r = rs.toDouble( &ok );
    if ( ! ok ) return Coordinate();
    QString ts = regexp.cap( 2 );
    double theta = KGlobal::locale()->readNumber( ts, &ok );
    if ( ! ok ) theta = ts.toDouble( &ok );
    if ( ! ok ) return Coordinate();
    theta *= M_PI;
    theta /= 180;
    return Coordinate( cos( theta ) * r, sin( theta ) * r );
  }
  else return Coordinate();
}

void PolarCoords::drawGrid( KigPainter& p, bool showgrid, bool showaxes ) const
{
  p.setWholeWinOverlay();

  // this instruction in not necessary, but there is a little
  // optimization when there are no grid and no axes.
  if ( !( showgrid || showaxes ) )
    return;

  // we multiply by sqrt( 2 ) cause we don't want to miss circles in
  // the corners, that intersect with the axes outside of the
  // screen..

  const double hmax = M_SQRT2*p.window().right();
  const double hmin = M_SQRT2*p.window().left();
  const double vmax = M_SQRT2*p.window().top();
  const double vmin = M_SQRT2*p.window().bottom();

  // the intervals:
  // we try to have one of them per 40 pixels or so..
  const int ntick = static_cast<int>(
    kigMax( hmax - hmin, vmax - vmin ) / p.pixelWidth() / 40 ) + 1;

  const double hrange = nicenum( hmax - hmin, false );
  const double vrange = nicenum( vmax - vmin, false );

  const double hd = nicenum( hrange / ( ntick - 1 ), true );
  const double vd = nicenum( vrange / ( ntick - 1 ), true );

  const double hgraphmin = floor( hmin / hd) * hd;
  const double hgraphmax = ceil( hmax / hd ) * hd;
  const double vgraphmin = floor( vmin / vd ) * vd;
  const double vgraphmax = ceil( vmax / vd ) * vd;

  const int hnfrac = kigMax( (int) - floor( log10( hd ) ), 0 );
  const int vnfrac = kigMax( (int) - floor( log10( vd ) ), 0 );
  const int nfrac = kigMax( hnfrac, vnfrac );

  /****** the grid lines ******/
  if ( showgrid )
  {
    double d = kigMin( hd, vd );
    double begin = kigMin( kigAbs( hgraphmin ), kigAbs( vgraphmin ) );
    if ( kigSgn( hgraphmin ) != kigSgn( hgraphmax ) && kigSgn( vgraphmin ) != kigSgn( vgraphmax ) )
      begin = d;
    double end = kigMax( hgraphmax, vgraphmax );

    // we also want the circles that don't fit entirely in the
    // screen..
    Coordinate c( 0, 0 );
    p.setPen( QPen( lightGray, 0, DotLine ) );
    for ( double i = begin; i <= end + d / 2; i += d )
      drawGridLine( p, c, fabs( i ) );
  }

  /****** the axes ******/
  if ( showaxes )
  {
    p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
    // x axis
    p.drawSegment( Coordinate( hmin, 0 ), Coordinate( hmax, 0 ) );
    // y axis
    p.drawSegment( Coordinate( 0, vmin ), Coordinate( 0, vmax ) );

    /****** the numbers ******/

    // x axis
    for( double i = hgraphmin; i <= hgraphmax + hd / 2; i += hd )
    {
      // we skip 0 since that would look stupid... (the axes going
      // through the 0 etc. )
      if( fabs( i ) < 1e-8 ) continue;

      QString is = KGlobal::locale()->formatNumber( fabs( i ), nfrac );
      p.drawText(
        Rect( Coordinate( i, 0 ), hd, -2*vd ).normalized(),
        is, AlignLeft | AlignTop );
    };
    // y axis...
    for ( double i = vgraphmin; i <= vgraphmax + vd / 2; i += vd )
    {
      if( fabs( i ) < 1e-8 ) continue;

      QString is = KGlobal::locale()->formatNumber( fabs( i ), nfrac );

      p.drawText ( Rect( Coordinate( 0, i ), hd, vd ).normalized(),
                   is, AlignBottom | AlignLeft
        );
    };
    // arrows on the ends of the axes...
    p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
    p.setBrush( QBrush( Qt::gray ) );
    std::vector<Coordinate> a;

    // the arrow on the right end of the X axis...
    a.reserve( 3 );
    double u = p.pixelWidth();
    a.push_back( Coordinate( hmax - 6 * u, -3 * u) );
    a.push_back( Coordinate( hmax, 0 ) );
    a.push_back( Coordinate( hmax - 6 * u, 3 * u ) );
//    p.drawPolygon( a, true );
    p.drawArea( a );

    // the arrow on the top end of the Y axis...
    a.clear();
    a.reserve( 3 );
    a.push_back( Coordinate( 3 * u, vmax - 6 * u ) );
    a.push_back( Coordinate( 0, vmax ) );
    a.push_back( Coordinate( -3 * u, vmax - 6 * u ) );
//    p.drawPolygon( a, true );
    p.drawArea( a );
  }; // if( showaxes )
}

QValidator* EuclideanCoords::coordinateValidator() const
{
  return new CoordinateValidator( false );
}

QValidator* PolarCoords::coordinateValidator() const
{
  return new CoordinateValidator( true );
}

QStringList CoordinateSystemFactory::names()
{
  QStringList ret;
  ret << i18n( "&Euclidean" )
      << i18n( "&Polar" );
  return ret;
}

CoordinateSystem* CoordinateSystemFactory::build( int which )
{
  if ( which == Euclidean )
    return new EuclideanCoords;
  else if ( which == Polar )
    return new PolarCoords;
  else return 0;
}

static const char euclideanTypeString[] = "Euclidean";
static const char polarTypeString[] = "Polar";

CoordinateSystem* CoordinateSystemFactory::build( const char* type )
{
  if ( std::string( euclideanTypeString ) == type )
    return new EuclideanCoords;
  if ( std::string( polarTypeString ) == type )
    return new PolarCoords;
  else return 0;
}

const char* EuclideanCoords::type() const
{
  return euclideanTypeString;
}

const char* PolarCoords::type() const
{
  return polarTypeString;
}

int EuclideanCoords::id() const
{
  return CoordinateSystemFactory::Euclidean;
}

int PolarCoords::id() const
{
  return CoordinateSystemFactory::Polar;
}

QString CoordinateSystemFactory::setCoordinateSystemStatement( int id )
{
  switch( id )
  {
  case Euclidean:
    return i18n( "Set Euclidean Coordinate System" );
  case Polar:
    return i18n( "Set Polar Coordinate System" );
  default:
    assert( false );
    return QString::null;
  }
}

Coordinate EuclideanCoords::snapToGrid( const Coordinate& c,
                                        const KigWidget& w ) const
{
  Rect rect = w.showingRect();
  // we recalc the interval stuff since there is no way to cache it..

  // this function is again inspired upon ( public domain ) code from
  // the first Graphics Gems book.  Credits to Paul S. Heckbert, who
  // wrote the "Nice number for graph labels" gem.

  const double hmax = rect.right();
  const double hmin = rect.left();
  const double vmax = rect.top();
  const double vmin = rect.bottom();

  // the number of intervals we would like to have:
  // we try to have one of them per 40 pixels or so..
  const int ntick = static_cast<int>(
    kigMax( hmax - hmin, vmax - vmin ) / w.pixelWidth() / 40. ) + 1;

  const double hrange = nicenum( hmax - hmin, false );
  const double vrange = nicenum( vmax - vmin, false );

  const double hd = nicenum( hrange / ( ntick - 1 ), true );
  const double vd = nicenum( vrange / ( ntick - 1 ), true );

  const double hgraphmin = ceil( hmin / hd) * hd;
  const double vgraphmin = ceil( vmin / vd ) * vd;

  const double nx = qRound( ( c.x - hgraphmin ) / hd ) * hd + hgraphmin;
  const double ny = qRound( ( c.y - vgraphmin ) / vd ) * vd + vgraphmin;
  return Coordinate( nx, ny );
}

Coordinate PolarCoords::snapToGrid( const Coordinate& c,
                                    const KigWidget& w ) const
{
  // we reuse the drawGrid code to find

  // we multiply by sqrt( 2 ) cause we don't want to miss circles in
  // the corners, that intersect with the axes outside of the
  // screen..

  Rect r = w.showingRect();

  const double hmax = M_SQRT2 * r.right();
  const double hmin = M_SQRT2 * r.left();
  const double vmax = M_SQRT2 * r.top();
  const double vmin = M_SQRT2 * r.bottom();

  // the intervals:
  // we try to have one of them per 40 pixels or so..
  const int ntick = static_cast<int>(
    kigMax( hmax - hmin, vmax - vmin ) / w.pixelWidth() / 40 ) + 1;

  const double hrange = nicenum( hmax - hmin, false );
  const double vrange = nicenum( vmax - vmin, false );

  const double hd = nicenum( hrange / ( ntick - 1 ), true );
  const double vd = nicenum( vrange / ( ntick - 1 ), true );

  double d = kigMin( hd, vd );

  double dist = c.length();
  double ndist = qRound( dist / d ) * d;
  return c.normalize( ndist );
}

void PolarCoords::drawGridLine( KigPainter& p, const Coordinate& c,
                                double r ) const
{
  Rect rect = p.window();

  struct iterdata_t
  {
    int xd;
    int yd;
    Coordinate ( Rect::*point )() const;
    Coordinate ( Rect::*oppositepoint )() const;
    double horizAngle;
    double vertAngle;
  };

  static const iterdata_t iterdata[] =
    {
      { +1, +1, &Rect::topRight, &Rect::bottomLeft, 0, M_PI/2 },
      { -1, +1, &Rect::topLeft, &Rect::bottomRight, M_PI, M_PI / 2 },
      { -1, -1, &Rect::bottomLeft, &Rect::topRight, M_PI, 3*M_PI/2 },
      { +1, -1, &Rect::bottomRight, &Rect::topLeft, 2*M_PI, 3*M_PI/2 }
    };
  for ( int i = 0; i < 4; ++i )
  {
    int xd = iterdata[i].xd;
    int yd = iterdata[i].yd;
    Coordinate point = ( rect.*iterdata[i].point )();
    Coordinate opppoint = ( rect.*iterdata[i].oppositepoint )();
    double horizangle = iterdata[i].horizAngle;
    double vertangle = iterdata[i].vertAngle;

    if ( ( c.x - point.x )*xd > 0 || ( c.y - point.y )*yd > 0 )
      continue;
    if ( ( c.x - opppoint.x )*-xd > r || ( c.y - opppoint.y )*-yd > r )
      continue;

    int posdir = xd*yd;
    double hd = ( point.x - c.x )*xd;
    assert( hd >= 0 );
    if ( hd < r )
    {
      double anglediff = acos( hd/r );
      horizangle += posdir * anglediff;
    }

    hd = ( c.x - opppoint.x )*-xd;
    if ( hd >= 0 )
    {
      double anglediff = asin( hd/r );
      vertangle -= posdir * anglediff;
    }

    double vd = ( point.y - c.y )*yd;
    assert( vd >= 0 );
    if ( vd < r )
    {
      double anglediff = acos( vd/r );
      vertangle -= posdir * anglediff;
    }

    vd = ( c.y - opppoint.y ) * -xd;
    if ( vd >= 0 )
    {
      double anglediff = asin( hd/r );
      horizangle += posdir * anglediff;
    }

    p.drawArc( c, r, kigMin( horizangle, vertangle ), kigMax( horizangle, vertangle ) );
  }
//  p.drawCircle( c, r );
}
