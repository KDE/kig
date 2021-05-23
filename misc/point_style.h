//  SPDX-FileCopyrightText: 2015 David E. Narvaez <david.narvaez@computer.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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
if ( style == QLatin1String("Round") )
return Round;
else if ( style == QLatin1String("RoundEmpty") )
return RoundEmpty;
else if ( style == QLatin1String("Rectangular") )
return Rectangular;
else if ( style == QLatin1String("RectangularEmpty") )
return RectangularEmpty;
else if ( style == QLatin1String("Cross") )
return Cross;
return PointStyle::Round;
}

inline QString pointStyleToString( const PointStyle ps )
{
if ( ps == Round )
return QStringLiteral("Round");
else if ( ps == RoundEmpty )
return QStringLiteral("RoundEmpty");
else if ( ps == Rectangular )
return QStringLiteral("Rectangular");
else if ( ps == RectangularEmpty )
return QStringLiteral("RectangularEmpty");
else if ( ps == Cross )
return QStringLiteral("Cross");
assert( false );
return QString();
}
}

#endif //POINT_STYLE_H
