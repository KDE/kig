// Copyright (C)  2005  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#include "inversion_type.h"
#include "point_imp.h"
#include "circle_imp.h"
#include "bogus_imp.h"

#include "../misc/common.h"

#include <klocale.h>

static const ArgsParser::spec argsspecInvertPoint[] =
{
  { PointImp::stype(), I18N_NOOP( "Compute the inversion of this point" ),
    I18N_NOOP( "Select the point to invert..." ), false },
  { CircleImp::stype(), I18N_NOOP( "Invert with respect to this circle" ),
    I18N_NOOP( "Select the circle we want to invert against..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InvertPointType )

InvertPointType::InvertPointType()
  : ArgsParserObjectType( "InvertPoint", argsspecInvertPoint, 2 )
{
}

InvertPointType::~InvertPointType()
{
}

const InvertPointType* InvertPointType::instance()
{
  static const InvertPointType s;
  return &s;
}

const ObjectImpType* InvertPointType::resultId() const
{
  return PointImp::stype();
}

ObjectImp* InvertPointType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const CircleImp* c = static_cast<const CircleImp*>( args[1] );
  Coordinate center = c->center();
  Coordinate relp = static_cast<const PointImp*>( args[0] )->coordinate() - center;
  double radiussq = c->squareRadius();
  double normsq = relp.x*relp.x + relp.y*relp.y;
  if ( normsq == 0 ) return new InvalidImp;
  return new PointImp( center + (radiussq/normsq)*relp );
}

