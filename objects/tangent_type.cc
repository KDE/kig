// tangent_type.cc
// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "tangent_type.h"

#include "bogus_imp.h"
#include "conic_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "line_imp.h"

#include "../misc/common.h"
#include "../misc/conic-common.h"
//#include "../misc/calcpaths.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

static const char constructlinetangentpoint[] =
  I18N_NOOP( "Construct the line tangent to this point" );

static const ArgsParser::spec argsspecTangentConic[] =
{
  { ConicImp::stype(), I18N_NOOP( "Construct the line tangent wrt. this conic" ),
    I18N_NOOP( "Select the conic..." ), true },
  { PointImp::stype(), constructlinetangentpoint,
    I18N_NOOP( "Select the point for the tangent to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TangentConicType )

TangentConicType::TangentConicType()
  : ArgsParserObjectType( "TangentConic", argsspecTangentConic, 2 )
{
}

TangentConicType::~TangentConicType()
{
}

const TangentConicType* TangentConicType::instance()
{
  static const TangentConicType t;
  return &t;
}

ObjectImp* TangentConicType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const ConicImp* c = static_cast<const ConicImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !c->containsPoint( p, doc ) )
    return new InvalidImp;

  bool ok;
  const LineData tangent = calcConicPolarLine( c->cartesianData(), p, ok );
  
  if ( !ok )
    return new InvalidImp;
  
  return new LineImp( tangent );
}

const ObjectImpType* TangentConicType::resultId() const
{
  return LineImp::stype();
}

static const ArgsParser::spec argsspecTangentArc[] =
{
  { ArcImp::stype(), I18N_NOOP( "Construct the line tangent wrt. this arc" ),
    I18N_NOOP( "Select the arc..." ), true },
  { PointImp::stype(), constructlinetangentpoint,
    I18N_NOOP( "Select the point for the tangent to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TangentArcType )

TangentArcType::TangentArcType()
  : ArgsParserObjectType( "TangentArc", argsspecTangentArc, 2 )
{
}

TangentArcType::~TangentArcType()
{
}

const TangentArcType* TangentArcType::instance()
{
  static const TangentArcType t;
  return &t;
}

ObjectImp* TangentArcType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const ArcImp* arc = static_cast<const ArcImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !arc->containsPoint( p, doc ) )
    return new InvalidImp;

  Coordinate c = arc->center();
  double sqr = arc->radius();
  sqr *= sqr;
  ConicCartesianData data( 1.0, 1.0, 0.0, -2*c.x, -2*c.y, c.x*c.x + c.y*c.y - sqr );

  bool ok;
  const LineData tangent = calcConicPolarLine( data, p, ok );
  
  if ( !ok )
    return new InvalidImp;

  return new LineImp( tangent );
}

const ObjectImpType* TangentArcType::resultId() const
{
  return LineImp::stype();
}
