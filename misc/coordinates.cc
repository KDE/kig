#include "coordinates.h"

EuclideanCoords::EuclideanCoords( int minX, int minY, int width, int height )
  : mMinX ( minX ),
    mMinY ( minY ),
    mWidth ( width ),
    mHeight ( height )
{
}

Point EuclideanCoords::fromScreen(const QPoint& pt, const QRect& r) const
{
  double nX = pt.x() * mWidth;
  nX /= r.width();
  double nY = pt.y() * mHeight;
  nY /= r.height();
  return Point ( nX, nY );
};

QPoint EuclideanCoords::toScreen(const Point& pt, const QRect& r) const
{
  double nX = pt.getX() * r.width();
  nX /= mWidth;
  double nY = pt.getY() * r.height();
  nY /= mHeight;
  return QPoint ( nX, nY );
};

void EuclideanCoords::drawGrid( QPainter& p )
{
  QRect size = p.window();
  // this grid comes from KGeo
  p.save();
  p.setWindow(-size.width() / 2, -size.height() / 2, size.width(), size.height());
  //gridLines:
  const int unitInPixel = size.width() / mWidth;
  p.setPen( QPen( lightGray, 0.5, DotLine ) );
  // vertical lines...
  for ( int i = mMinX; i <= mMinX + mWidth; ++i )
    p.drawLine
      (
       i * unitInPixel,
       mMinY * unitInPixel,
       i * unitInPixel,
       mMaxY * unitInPixel
      );
  // horizontal lines...
  for ( int i = mMinY; i <= mMinY + mHeight; ++i )
    p.drawLine
      (
       mMinX * unitInPixel,
       i * unitInPixel,
       mMaxX * unitInPixel,
       i * unitInPixel
      );
  
  // axes:
  p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
  p.drawLine( mMinX * unitInPixel, 0, mMaxX * unitInPixel, 0 );
  p.drawLine( 0, mMinY * unitInPixel, 0, mMaxY * unitInPixel );

  // the numbers:
  // x axis
  for ( int i = mMinX; i <= mMinX + mWidth; ++i )
    if ( i != 0 )
      p.drawText
	(
	 (i-1) * unitInPixel,
	 2,
	 2 * unitInPixel,
	 unitInPixel,
	 AlignHCenter | AlignTop,
	 QString().setNum( k )
	);
    }
  // y axis...
  for ( int i = mMinY; i <= mMinY + mHeight; ++i )
    if ( i != 0 )
      p.drawText
        (
	 2,
	 ( i * unitInPixel ) - 13,
	 24,
	 24,
	 AlignCenter,
	 QString().setNum( -i ) );
	}
    }
  int right = (mMinX + mWidth) * unitInPixel;
  int top = (mMinY + mHeight) * unitInPixel;
  p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
  p.setBrush( Qt::gray );
  QPointArray a;
  a.setPoints ( 3, right + 6, - 3, right + 12 , 0, right + 6, 3 );
  p.drawPolygon( a, true );
  p.drawLine( right, 0, right + 5, 0 );
  a.setPoints ( 3, - 3, top - 6, 0, top - 13, 3, top - 6 );
  p.drawPolygon( a, true );
  p.drawLine( 0, top, 0, top - 6 );
  
  p.restore();
}
