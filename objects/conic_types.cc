// conic_types.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "conic_types.h"

#include "bogus_imp.h"
#include "conic_imp.h"
#include "point_imp.h"
#include "line_imp.h"
#include "../misc/conic-common.h"
#include "../misc/common.h"

static const struct ArgParser::spec argsspec5p[] =
{
  { ObjectImp::ID_PointImp, 5 }
};

ConicB5PType::ConicB5PType()
  : ObjectType( "conic", "ConicB5P", argsspec5p, 1 )
{
}

ConicB5PType::~ConicB5PType()
{
}

ObjectImp* ConicB5PType::calc(
  const Args& parents,
  const KigWidget& ) const
{
  assert( parents.size() <= 5 );
  std::vector<Coordinate> points;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  if ( points.empty() ) return new InvalidImp;
  else return new ConicImpCart(
    calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry ) );
}

const ConicB5PType* ConicB5PType::instance()
{
  static const ConicB5PType t;
  return &t;
}

static const ArgParser::spec argsspecllp[] =
{
  { ObjectImp::ID_LineImp, 2 },
  { ObjectImp::ID_PointImp, 1 }
};

ConicBAAPType::ConicBAAPType()
  : ObjectType( "conic", "ConicBAAP", argsspecllp, 2 )
{
}

ConicBAAPType::~ConicBAAPType()
{
}

const ConicBAAPType* ConicBAAPType::instance()
{
  static const ConicBAAPType t;
  return &t;
}

ObjectImp* ConicBAAPType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 3 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  assert( parsed[0]->inherits( ObjectImp::ID_LineImp ) );
  assert( parsed[1]->inherits( ObjectImp::ID_LineImp ) );
  assert( parsed[2]->inherits( ObjectImp::ID_PointImp ) );
  const LineData la = static_cast<const LineImp*>( parsed[0] )->data();
  const LineData lb = static_cast<const LineImp*>( parsed[1] )->data();
  const Coordinate c = static_cast<const PointImp*>( parsed[2] )->coordinate();

  return new ConicImpCart( calcConicByAsymptotes( la, lb, c ) );
}

ObjectImp* ConicBFFPType::calc(
  const Args& parents, const KigWidget& ) const
{
  std::vector<Coordinate> cs;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      cs.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  if ( cs.size() < 2 ) return new InvalidImp;
  else return new ConicImpPolar( calcConicBFFP( cs, type() ) );
}

ConicBFFPType::ConicBFFPType( const char* fullname )
  : ObjectABCType( "conic", fullname )
{
}

ConicBFFPType::~ConicBFFPType()
{
}

EllipseBFFPType::EllipseBFFPType()
  : ConicBFFPType( "EllipseBFFP" )
{
}

EllipseBFFPType::~EllipseBFFPType()
{
}

int EllipseBFFPType::type() const
{
  return 1;
}

const EllipseBFFPType* EllipseBFFPType::instance()
{
  static const EllipseBFFPType t;
  return &t;
}

HyperbolaBFFPType::HyperbolaBFFPType()
  : ConicBFFPType( "HyperbolaBFFP" )
{
}

HyperbolaBFFPType::~HyperbolaBFFPType()
{
}

const HyperbolaBFFPType* HyperbolaBFFPType::instance()
{
  static const HyperbolaBFFPType t;
  return &t;
}

int HyperbolaBFFPType::type() const
{
  return -1;
}

const ConicBDFPType* ConicBDFPType::instance()
{
  static const ConicBDFPType t;
  return &t;
}

static const struct ArgParser::spec argsspeclpp[] =
{
  { ObjectImp::ID_PointImp, 2 },
  { ObjectImp::ID_LineImp, 1 }
};

ConicBDFPType::ConicBDFPType()
  : ObjectType( "conic", "ConicBDFP", argsspeclpp, 2 )
{
}

ConicBDFPType::~ConicBDFPType()
{
}

ObjectImp* ConicBDFPType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );

  if ( ! parsed[2] || ! parsed[2]->inherits( ObjectImp::ID_LineImp ) )
    return new InvalidImp;
  if ( ! parsed[0] || ! parsed[0]->inherits( ObjectImp::ID_PointImp ) )
    return new InvalidImp;
  if ( parsed[1] && !parsed[1]->inherits( ObjectImp::ID_PointImp ) )
    return new InvalidImp;
  const LineData line = static_cast<const LineImp*>( parsed[2] )->data();
  const Coordinate focus =
    static_cast<const PointImp*>( parsed[0] )->coordinate();
  Coordinate point;
  if ( parsed[1] )
    point = static_cast<const PointImp*>( parsed[1] )->coordinate();
  else
  {
    /* !!!! costruisci point come punto medio dell'altezza tra fuoco e d. */
    Coordinate ba = line.dir();
    Coordinate fa = focus - line.b;
    double balsq = ba.x*ba.x + ba.y*ba.y;
    double scal = (fa.x*ba.x + fa.y*ba.y)/balsq;
    point = 0.5*(line.a + focus + scal*ba);
  };
  return new ConicImpPolar( calcConicBDFP( line, focus, point ) );
}

ParabolaBTPType::ParabolaBTPType()
  : ObjectABCType( "conic", "ParabolaBTP" )
{
}

ParabolaBTPType::~ParabolaBTPType()
{
}

const ParabolaBTPType* ParabolaBTPType::instance()
{
  static const ParabolaBTPType t;
  return &t;
}

ObjectImp* ParabolaBTPType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );
  if ( points.size() < 2 ) return new InvalidImp;
  return new ConicImpCart(
    calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry ) );
}

static const ArgParser::spec argsspeccl[] =
{
  { ObjectImp::ID_ConicImp, 1 },
  { ObjectImp::ID_LineImp, 1 }
};

ConicPolarPointType::ConicPolarPointType()
  : ObjectType( "point", "ConicPolarPoint", argsspeccl, 2 )
{
}

ConicPolarPointType::~ConicPolarPointType()
{
}

const ConicPolarPointType* ConicPolarPointType::instance()
{
  static const ConicPolarPointType t;
  return &t;
}

ObjectImp* ConicPolarPointType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  if ( parsed.size() < 2 || ! parsed[0] || ! parsed[1] ) return new InvalidImp;
  const ConicCartesianData c = static_cast<const ConicImp*>( parsed[0] )->cartesianData();
  const LineData l = static_cast<const LineImp*>( parsed[1] )->data();
  bool valid = true;
  const Coordinate p = calcConicPolarPoint( c, l, valid );
  if ( valid ) return new PointImp( p );
  else return new InvalidImp;
}

static const ArgParser::spec argsspeccp[] =
{
  { ObjectImp::ID_ConicImp, 1 },
  { ObjectImp::ID_PointImp, 1 }
};

ConicPolarLineType::ConicPolarLineType()
  : ObjectType( "conic", "ConicPolarLine", argsspeccp, 2 )
{
}

ConicPolarLineType::~ConicPolarLineType()
{
}

const ConicPolarLineType* ConicPolarLineType::instance()
{
  static const ConicPolarLineType t;
  return &t;
}

ObjectImp* ConicPolarLineType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  if ( parsed.size() < 2 || ! parsed[0] || ! parsed[1] ) return new InvalidImp;
  const ConicCartesianData c = static_cast<const ConicImp*>( parsed[0] )->cartesianData();
  const Coordinate p = static_cast<const PointImp*>( parsed[1] )->coordinate();
  bool valid = true;
  const LineData l = calcConicPolarLine( c, p, valid );
  if ( valid ) return new LineImp( l );
  else return new InvalidImp;
}
