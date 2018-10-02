//  Copyright 2015  David E. Narvaez <david.narvaez@computer.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

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
