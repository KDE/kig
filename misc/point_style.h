#ifndef POINT_STYLE_H
#define POINT_STYLE_H

#include <cassert>

namespace Kig {
  enum PointStyle {
    Round = 0,
    RoundEmpty,
    Rectangular,
    RectangularEmpty,
    Cross,
    NumberOfPointStyles
  };
  
  /**
   * Note that this returns a valid point style in every case, even if
   * the given \p style string is unknown. In that case it returns a
   * default value.
   */
inline PointStyle pointStyleFromString( const QString& style )
{
if ( style == "Round" )
return Round;
else if ( style == "RoundEmpty" )
return RoundEmpty;
else if ( style == "Rectangular" )
return Rectangular;
else if ( style == "RectangularEmpty" )
return RectangularEmpty;
else if ( style == "Cross" )
return Cross;
return PointStyle::Round;
}

inline QString pointStyleToString( const PointStyle ps )
{
if ( ps == Round )
return "Round";
else if ( ps == RoundEmpty )
return "RoundEmpty";
else if ( ps == Rectangular )
return "Rectangular";
else if ( ps == RectangularEmpty )
return "RectangularEmpty";
else if ( ps == Cross )
return "Cross";
assert( false );
return QString();
}
}

#endif //POINT_STYLE_H
