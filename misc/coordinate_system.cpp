/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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

#include <qpainter.h>
#include <qregexp.h>

#include <cmath>

// this is in math.h, but include cmath doesn't work nicely for me..
// #include <cmath>
// extern "C" {
// double fabs(double);
// };

EuclideanCoords::EuclideanCoords()
{
}

QString EuclideanCoords::fromScreen(const Coordinate& p) const
{
  return QString("(")+p.x+','+p.y+')';
};

Coordinate EuclideanCoords::toScreen(const QString& s, bool& ok) const
{
  QRegExp r("\\(([^,]+),([^\\)]+)\\)");
  ok = r.match(s);
  if (ok)
  {
    return Coordinate(r.cap(1).toDouble(&ok), r.cap(2).toDouble(&ok));
  }
  else return Coordinate();
};


void EuclideanCoords::drawGrid( KigPainter& p ) const
{
  // the intervals:
  const double hInterval =
    pow( 10, floor( log10( p.window().width() / 2.5 ) ) );
  const double vInterval = hInterval;


  // this grid comes largely from KGeo
  const double dMinX = p.window().left();
  const int iMinX = static_cast<int>( dMinX / hInterval );
  const double dMaxX = p.window().right();
  const int iMaxX = static_cast<int>( dMaxX / hInterval );
  const double dMinY = p.window().bottom();
  const int iMinY = static_cast<int>( dMinY / vInterval );
  const double dMaxY = p.window().top();
  const int iMaxY = static_cast<int>( dMaxY / vInterval );

  /****** the grid lines ******/
  p.setPen( QPen( lightGray, 0, DotLine ) );
  // vertical lines...
  for ( int i = iMinX; i <= iMaxX; ++i )
    p.drawSegment( Coordinate( i * hInterval, dMinY ),
		   Coordinate( i * hInterval, dMaxY ) );
  // horizontal lines...
  for ( int i = iMinY; i <= iMaxY; ++i )
    p.drawSegment( Coordinate( dMinX, i * vInterval ),
		   Coordinate( dMaxX, i * vInterval ) );

  /****** the axes ******/
  p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
  // x axis
  p.drawSegment( Coordinate( dMinX, 0 ), Coordinate( dMaxX, 0 ) );
  // y axis
  p.drawSegment( Coordinate( 0, dMinY ), Coordinate( 0, dMaxY ) );

  /****** the numbers ******/

  // we don't draw all numbers...
  const int hStep = (iMaxX - iMinX) >= 10 ? 2 : 1;
  const int vStep = (iMaxY - iMinY) >= 10 ? 2 : 1;

  // x axis
  for( int i = iMinX; i <= iMaxX; i += hStep )
    {
      // we skip 0 since that would look stupid... (the axes going
      // through the 0 etc. )
      if( i == 0 ) continue;

      p.drawText(
		 Rect( Coordinate( i * hInterval, 0 ), hStep*hInterval, -vInterval ).normalized(),
		 QString().setNum( i * hInterval ),
		 AlignLeft | AlignTop
		 );
    };
  // y axis...
  for ( int i = iMinY; i <= iMaxY; i += vStep )
    {
      if( i == 0 ) continue;
      p.drawText ( Rect( Coordinate( 0, i * vInterval ), vStep*hInterval, vInterval ).normalized(),
		   QString().setNum( i * vInterval ),
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
  a.push_back( Coordinate( dMaxX - 6 * u, -3 * u) );
  a.push_back( Coordinate( dMaxX, 0 ) );
  a.push_back( Coordinate( dMaxX - 6 * u, 3 * u ) );
  p.drawPolygon( a, true );
  //  p.drawLine( right, 0, right + 5, 0 );

  // the arrow on the top end of the Y axis...
  a.clear();
  a.reserve( 3 );
  a.push_back( Coordinate( 3 * u, dMaxY - 6 * u ) );
  a.push_back( Coordinate( 0, dMaxY ) );
  a.push_back( Coordinate( -3 * u, dMaxY - 6 * u ) );
  p.drawPolygon( a, true );
  //  p.drawLine( 0, top, 0, top - 6 );
}

QString EuclideanCoords::coordinateFormatNotice() const
{
  return i18n( "Enter coordinates in the following form: \"(x,y)\", where "
               "x is the x coordinate, and y is the y coordinate." );
}
