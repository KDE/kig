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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "coordinate_system.h"

#include "../kig/kig_part.h"

#include "i18n.h"
#include "common.h"

#include <qpainter.h>
#include <qregexp.h>
#include <kdebug.h>

#include <cmath>
#include <string>

using std::ceil;
using std::floor;
using std::pow;
using std::max;
using std::log10;

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
  double m = max( sr.width(), sr.height() );
  int l;
  if ( m < 1 ) l = 3 - (int) log10( m );
  else l = (int) log10( m ) + 2;
  return QString::fromLatin1("( %1; %2 )").arg(p.x, 0, 'g', l ).arg(p.y, 0, 'g', l );
};

Coordinate EuclideanCoords::toScreen(const QString& s, bool& ok) const
{
  QRegExp r("^([^,]+),([^\\)]+)$");
  ok = ( r.search(s) == 0 );
  if (ok)
  {
    return Coordinate(r.cap(1).toDouble(&ok), r.cap(2).toDouble(&ok));
  }
  else return Coordinate();
};

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
};

void EuclideanCoords::drawGrid( KigPainter& p, bool showgrid, bool showaxes ) const
{
  p.setWholeWinOverlay();

  // this function is inspired upon ( public domain ) code from the
  // first Graphics Gems book.  Credits to Paul S. Heckbert, who wrote
  // the "Nice number for graph labels" gem.

  const double hmax = p.window().right();
  const double hmin = p.window().left();
  const double vmax = p.window().top();
  const double vmin = p.window().bottom();

  // the number of intervals we would like to have:
  // we try to have one of them per 25 pixels or so..
  const int ntick = static_cast<int>(
    kigMax( hmax - hmin, vmax - vmin ) / p.pixelWidth() / 25. ) + 1;

  const double hrange = nicenum( hmax - hmin, false );
  const double vrange = nicenum( vmax - vmin, false );

  const double hd = nicenum( hrange / ( ntick - 1 ), true );
  const double vd = nicenum( vrange / ( ntick - 1 ), true );

  const double hgraphmin = floor( hmin / hd) * hd;
  const double hgraphmax = ceil( hmax / hd ) * hd;
  const double vgraphmin = floor( vmin / vd ) * vd;
  const double vgraphmax = ceil( vmax / vd ) * vd;

  // this would be the number of decimals we show, but Qt has no
  // facilities for applying this..
//   const int hnfrac = max( (int) - floor( log10( hd ) ), 0 );
//   const int vnfrac = max( (int) - floor( log10( vd ) ), 0 );

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
        QString().setNum( i ), AlignLeft | AlignTop
        );
    };
    // y axis...
    for ( double i = vgraphmin; i <= vgraphmax + vd/2; i += vd )
    {
      if( fabs( i ) < 1e-8 ) continue;
      p.drawText ( Rect( Coordinate( 0, i ), hd, vd ).normalized(),
                   QString().setNum( i ), AlignBottom | AlignLeft
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
    p.drawPolygon( a, true );

    // the arrow on the top end of the Y axis...
    a.clear();
    a.reserve( 3 );
    a.push_back( Coordinate( 3 * u, vmax - 6 * u ) );
    a.push_back( Coordinate( 0, vmax ) );
    a.push_back( Coordinate( -3 * u, vmax - 6 * u ) );
    p.drawPolygon( a, true );
  }; // if( showaxes )
}

QString EuclideanCoords::coordinateFormatNotice() const
{
  return i18n( "Enter coordinates in the following form: \"x,y\", where "
               "x is the x coordinate, and y is the y coordinate." );
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
  double m = max( sr.width(), sr.height() );
  int l;
  if ( m < 1 ) l = 3 - (int) log10( m );
  else l = (int) log10( m ) + 2;

  double r = pt.length();
  double theta = atan2( pt.y, pt.x );

  return QString::fromLatin1("( %1; %2 )").arg(r, 0, 'g', l ).arg( theta );
}

QString PolarCoords::coordinateFormatNotice() const
{
  return i18n( "Enter coordinates in the following form: \"r, \xCE\xB8\", where " // \xCE\xB8 is utf8 for the greek theta sign..
               "r and theta are the polar coordinates." );
}

Coordinate PolarCoords::toScreen(const QString& s, bool& ok) const
{
  QRegExp regexp("^([^,]+),([^\\)]+)$");
  ok = ( regexp.search(s) == 0 );
  if (ok)
  {
    double r = regexp.cap( 1 ).toDouble( &ok );
    if ( ! ok ) return Coordinate();
    double theta = regexp.cap(2).toDouble( &ok );
    if ( ! ok ) return Coordinate();
    return Coordinate( cos( theta )*r, sin( theta ) * r );
  }
  else return Coordinate();
}

void PolarCoords::drawGrid( KigPainter& p, bool showgrid, bool showaxes ) const
{
  p.setWholeWinOverlay();

  // we multiply by sqrt( 2 ) cause we don't want to miss circles in
  // the corners, that intersect with the axes outside of the
  // screen..
  const double sqrt2 = 1.4142135623;

  const double hmax = sqrt2*p.window().right();
  const double hmin = sqrt2*p.window().left();
  const double vmax = sqrt2*p.window().top();
  const double vmin = sqrt2*p.window().bottom();

  // the intervals:
  // we try to have one of them per 25 pixels or so..
  const int ntick = static_cast<int>(
    kigMax( hmax - hmin, vmax - vmin ) / p.pixelWidth() / 25 ) + 1;

  const double hrange = nicenum( hmax - hmin, false );
  const double vrange = nicenum( vmax - vmin, false );

  const double hd = nicenum( hrange / ( ntick - 1 ), true );
  const double vd = nicenum( vrange / ( ntick - 1 ), true );

  const double hgraphmin = floor( hmin / hd) * hd;
  const double hgraphmax = ceil( hmax / hd ) * hd;
  const double vgraphmin = floor( vmin / vd ) * vd;
  const double vgraphmax = ceil( vmax / vd ) * vd;

  /****** the grid lines ******/
  if ( showgrid )
  {
    double begin = kigMin( hgraphmin, vgraphmin );
    double end = kigMax( hgraphmax, vgraphmax );
    double d = kigMin( hd, vd );

    // we also want the circles that don't fit entirely in the
    // screen..
    p.setPen( QPen( lightGray, 0, DotLine ) );
    for ( double i = begin; i <= end + d / 2; i += d )
      p.drawCircle( Coordinate( 0, 0 ), fabs( i ) );
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
        QString().setNum( fabs( i ) ), AlignLeft | AlignTop
        );
    };
    // y axis...
    for ( double i = vgraphmin; i <= vgraphmax + vd / 2; i += vd )
    {
      if( fabs( i ) < 1e-8 ) continue;
      p.drawText ( Rect( Coordinate( 0, i ), hd, vd ).normalized(),
                   QString().setNum( fabs( i ) ), AlignBottom | AlignLeft
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
    p.drawPolygon( a, true );

    // the arrow on the top end of the Y axis...
    a.clear();
    a.reserve( 3 );
    a.push_back( Coordinate( 3 * u, vmax - 6 * u ) );
    a.push_back( Coordinate( 0, vmax ) );
    a.push_back( Coordinate( -3 * u, vmax - 6 * u ) );
    p.drawPolygon( a, true );
  }; // if( showaxes )
}

static const char* euclideanTypeString = "Euclidean";
static const char* polarTypeString = "Polar";

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
