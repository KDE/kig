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
#include "circle_imp.h"
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

ObjectImp* ConicBAAPType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 3 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  assert( parsed[0]->inherits( ObjectImp::ID_LineImp ) );
  assert( parsed[1]->inherits( ObjectImp::ID_LineImp ) );
  assert( parsed[2]->inherits( ObjectImp::ID_PointImp ) );
  const LineData la = static_cast<const AbstractLineImp*>( parsed[0] )->data();
  const LineData lb = static_cast<const AbstractLineImp*>( parsed[1] )->data();
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

HyperbolaBFFPType::HyperbolaBFFPType()
  : ConicBFFPType( "HyperbolaBFFP" )
{
}

HyperbolaBFFPType::~HyperbolaBFFPType()
{
}

int HyperbolaBFFPType::type() const
{
  return -1;
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
  const LineData line = static_cast<const AbstractLineImp*>( parsed[2] )->data();
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

ObjectImp* ConicPolarPointType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  if ( parsed.size() < 2 || ! parsed[0] || ! parsed[1] ) return new InvalidImp;
  const ConicCartesianData c = static_cast<const ConicImp*>( parsed[0] )->cartesianData();
  const LineData l = static_cast<const AbstractLineImp*>( parsed[1] )->data();
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

static const ArgParser::spec argsspecc[] =
{
  { ObjectImp::ID_ConicImp, 1 }
};

ConicDirectrixType::ConicDirectrixType()
  : ObjectType( "line", "ConicDirectrix", argsspecc, 1 )
{
}

ConicDirectrixType::~ConicDirectrixType()
{
}

ObjectImp* ConicDirectrixType::calc( const Args& parents,
                                     const KigWidget& ) const
{
  if ( parents.size() != 1 ) return new InvalidImp;

  const ConicPolarData data =
    static_cast<const ConicImp*>( parents[0] )->polarData();

  double ec = data.ecostheta0;
  double es = data.esintheta0;
  double eccsq = ec*ec + es*es;

  Coordinate a = data.focus1 - data.pdimen/eccsq*Coordinate(ec,es);
  Coordinate b = a + Coordinate(-es,ec);
  return new LineImp( a, b );
}

static const ArgParser::spec argsspec4p[] =
{
  { ObjectImp::ID_PointImp, 4 }
};

EquilateralHyperbolaB4PType::EquilateralHyperbolaB4PType()
  : ObjectType( "conic", "EquilateralHyperbolaB4P", argsspec4p, 1 )
{
}

EquilateralHyperbolaB4PType::~EquilateralHyperbolaB4PType()
{
}

ObjectImp* EquilateralHyperbolaB4PType::calc( const Args& parents,
                                              const KigWidget& ) const
{
  std::vector<Coordinate> pts;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      pts.push_back( static_cast<const PointImp*>( *i )->coordinate() );
  return new ConicImpCart( calcConicThroughPoints( pts, equilateral ) );
}

ParabolaBDPType::ParabolaBDPType()
  : ObjectLPType( "conic", "ParabolaBDP" )
{
}

ParabolaBDPType::~ParabolaBDPType()
{
}

ObjectImp* ParabolaBDPType::calc( const LineData& l,
                                  const Coordinate& c ) const
{
  ConicPolarData ret;
  Coordinate ldir = l.dir();
  ldir = ldir.normalize();
  ret.focus1 = c;
  ret.ecostheta0 = - ldir.y;
  ret.esintheta0 = ldir.x;
  Coordinate fa = c - l.a;
  ret.pdimen = fa.y*ldir.x - fa.x*ldir.y;
  ConicImpPolar* r = new ConicImpPolar( ret );
  kdDebug() << k_funcinfo << r->conicTypeString() << endl;
  return r;
}

static const ArgParser::spec argsspecci[] =
{
  { ObjectImp::ID_ConicImp, 1 },
  { ObjectImp::ID_IntImp, 1 }
};

ConicAsymptoteType::ConicAsymptoteType()
  : ObjectType( "line", "ConicAsymptote", argsspecci, 2 )
{
}

ConicAsymptoteType::~ConicAsymptoteType()
{
}

ObjectImp* ConicAsymptoteType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( !p[0] || ! p[0] ) return new InvalidImp;
  bool valid = true;
  const LineData ret = calcConicAsymptote(
    static_cast<const ConicImp*>( p[0] )->cartesianData(),
    static_cast<const IntImp*>( p[1] )->data(),
    valid );
  if ( valid ) return new LineImp( ret );
  else return new InvalidImp;
}

static const ArgParser::spec argsspecccl[] =
{
  { ObjectImp::ID_ConicImp, 2 },
  { ObjectImp::ID_IntImp, 2 }
};

ConicRadicalType::ConicRadicalType()
  : ObjectType( "line", "ConicRadical", argsspecccl, 2 )
{
}

ObjectImp* ConicRadicalType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() != 4 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( !p[0] || ! p[1] || ! p[2] || !p[3] ) return new InvalidImp;
  if ( p[0]->inherits( ObjectImp::ID_CircleImp ) &&
       p[1]->inherits( ObjectImp::ID_CircleImp ) )
  {
    if( static_cast<const IntImp*>( p[2] )->data() != 1 )
      return new InvalidImp;
    else
    {
      const CircleImp* c1 = static_cast<const CircleImp*>( p[0] );
      const CircleImp* c2 = static_cast<const CircleImp*>( p[1] );
      const Coordinate a = calcCircleRadicalStartPoint(
        c1->center(), c2->center(), c1->squareRadius(), c2->squareRadius()
        );
      return new LineImp( a, calcPointOnPerpend(
          LineData( c1->center(), c2->center() ), a ) );
    };
  }
  else
  {
    bool valid = true;
    const LineData l = calcConicRadical(
      static_cast<const ConicImp*>( p[0] )->cartesianData(),
      static_cast<const ConicImp*>( p[1] )->cartesianData(),
      static_cast<const IntImp*>( p[2] )->data(),
      static_cast<const IntImp*>( p[3] )->data(), valid );
    if ( valid ) return new LineImp( l );
    else return new InvalidImp;
  };
}

ConicRadicalType::~ConicRadicalType()
{
}

ObjectType* ConicB5PType::copy() const
{
  return new ConicB5PType;
}

ObjectType* ConicBAAPType::copy() const
{
  return new ConicBAAPType;
}

ObjectType* EllipseBFFPType::copy() const
{
  return new EllipseBFFPType;
}

ObjectType* HyperbolaBFFPType::copy() const
{
  return new HyperbolaBFFPType;
}

ObjectType* ConicBDFPType::copy() const
{
  return new ConicBDFPType;
}

ObjectType* ParabolaBTPType::copy() const
{
  return new ParabolaBTPType;
}

ObjectType* EquilateralHyperbolaB4PType::copy() const
{
  return new EquilateralHyperbolaB4PType;
}

ObjectType* ConicPolarPointType::copy() const
{
  return new ConicPolarPointType;
}

ObjectType* ConicPolarLineType::copy() const
{
  return new ConicPolarLineType;
}

ObjectType* ConicDirectrixType::copy() const
{
  return new ConicDirectrixType;
}

ObjectType* ParabolaBDPType::copy() const
{
  return new ParabolaBDPType;
}

ObjectType* ConicAsymptoteType::copy() const
{
  return new ConicAsymptoteType;
}

ObjectType* ConicRadicalType::copy() const
{
  return new ConicRadicalType;
}
