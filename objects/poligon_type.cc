// Copyright (C)  2003  Maurizio Paolini <paolini@dmf.unicatt.it>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "poligon_type.h"

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "polygon_imp.h"

#include "../misc/common.h"

#include <klocale.h>
#include <cmath>
#include <vector>

static const char constructpoligonthroughpointstat[] = I18N_NOOP( "Construct a polygon with this vertex" );

static const char constructpoligonwithcenterstat[] = I18N_NOOP( "Construct a polygon with this center" );

static const ArgsParser::spec argsspecPoligonBCV[] =
{
  { PointImp::stype(), constructpoligonwithcenterstat,
    I18N_NOOP( "Select the center of the new polygon..." ), false },
  { PointImp::stype(), constructpoligonthroughpointstat,
    I18N_NOOP( "Select a vertex for the new polygon..." ), true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PoligonBCVType )

PoligonBCVType::PoligonBCVType()
  : ArgsParserObjectType( "PoligonBCV", argsspecPoligonBCV, 3 )
{
}

PoligonBCVType::~PoligonBCVType()
{
}

const PoligonBCVType* PoligonBCVType::instance()
{
  static const PoligonBCVType s;
  return &s;
}

ObjectImp* PoligonBCVType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const Coordinate center = 
        static_cast<const PointImp*>( parents[0] )->coordinate();
  const Coordinate vertex = 
        static_cast<const PointImp*>( parents[1] )->coordinate();
  const int sides =
        static_cast<const IntImp*>( parents[2] )->data();

  std::vector<Coordinate> vertexes;

  double dx = vertex.x - center.x;
  double dy = vertex.y - center.y;

  for ( int i = 1; i <= sides; i++ )
  {
    double alfa = 2*M_PI/sides;
    double theta1 = alfa*i - alfa;
    double ctheta1 = cos(theta1);
    double stheta1 = sin(theta1);

    Coordinate v1 = center + Coordinate( ctheta1*dx - stheta1*dy,
                                         stheta1*dx + ctheta1*dy );
    vertexes.push_back( v1 );
  }
  return new PolygonImp( vertexes );
}

const ObjectImpType* PoligonBCVType::resultId() const
{
  return SegmentImp::stype();
}
