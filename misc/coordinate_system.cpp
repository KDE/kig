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
    static_cast<int>( pow( 10, floor( log10( p.window().width() / 1.5 ) ) ) );
  const double vInterval =
    static_cast<int>( pow( 10, floor( log10( p.window().height() / 1.5 ) ) ) );

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
  // x axis
  for( int i = iMinX; i <= iMaxX; ++i )
    {
      // we skip 0 since that would look stupid... (the axes going
      // through the 0 etc. )
      if( i == 0 ) continue;
      
      p.drawText(
		 Rect( Coordinate( i * hInterval, 0 ), hInterval, -vInterval ).normalized(),
		 QString().setNum( i * hInterval ),
		 AlignLeft | AlignTop
		 );
    };
  // y axis...
  for ( int i = iMinY; i <= iMaxY; ++i )
    {
      if( i == 0 ) continue;
      p.drawText ( Rect( Coordinate( 0, i * vInterval ), hInterval, vInterval ).normalized(),
		   QString().setNum( i * vInterval ),
		   AlignBottom | AlignLeft
		   );
    };
  /*
  // arrows on the axes...
  p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
  p.setBrushColor( Qt::gray );
  std::vector<Coordinate> a;
  a.reserve( 3 );
  a.push_back( Coordinate( mMaxX - 0.6, -0.3 ) );
  a.push_back( Coordinate( mMaxX, 0 ) );
  a.push_back( Coordinate( mMaxX - 0.6, 0 + 0.3 ) );
  p.drawPolygon( a, true );
  //  p.drawLine( right, 0, right + 5, 0 );
  a.clear();
  a.reserve( 3 );
  a.push_back( Coordinate( 0.3, mMaxY - 0.6 ) );
  a.push_back( Coordinate( 0, mMaxY) );
  a.push_back( Coordinate( 0.3, mMaxY - 6 ) );
  p.drawPolygon( a, true );
  //  p.drawLine( 0, top, 0, top - 6 );
  */
}
