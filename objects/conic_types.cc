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

static const char* constructstatement = I18N_NOOP( "Construct a conic through this point" );

static const struct ArgParser::spec argsspec5p[] =
{
  { ObjectImp::ID_PointImp, constructstatement },
  { ObjectImp::ID_PointImp, constructstatement },
  { ObjectImp::ID_PointImp, constructstatement },
  { ObjectImp::ID_PointImp, constructstatement },
  { ObjectImp::ID_PointImp, constructstatement }
};

ConicB5PType::ConicB5PType()
  : ArgparserObjectType( "ConicB5P", argsspec5p, 5 )
{
}

ConicB5PType::~ConicB5PType()
{
}

ObjectImp* ConicB5PType::calc( const Args& parents, const KigDocument& ) const
{
  assert( parents.size() <= 5 );
  std::vector<Coordinate> points;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  if ( points.size() != parents.size() ) return new InvalidImp;
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
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct a conic with this asymptote" ) },
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct a conic with this asymptote" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a conic through this point" ) }
};

ConicBAAPType::ConicBAAPType()
  : ArgparserObjectType( "ConicBAAP", argsspecllp, 3 )
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

ObjectImp* ConicBAAPType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 3 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  if ( ! parsed[0] || ! parsed[1] || ! parsed[2] )
    return new InvalidImp;
  assert( parsed[0]->inherits( ObjectImp::ID_LineImp ) );
  assert( parsed[1]->inherits( ObjectImp::ID_LineImp ) );
  assert( parsed[2]->inherits( ObjectImp::ID_PointImp ) );
  const LineData la = static_cast<const AbstractLineImp*>( parsed[0] )->data();
  const LineData lb = static_cast<const AbstractLineImp*>( parsed[1] )->data();
  const Coordinate c = static_cast<const PointImp*>( parsed[2] )->coordinate();

  return new ConicImpCart( calcConicByAsymptotes( la, lb, c ) );
}

ObjectImp* ConicBFFPType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  std::vector<Coordinate> cs;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      cs.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  if ( cs.size() != parents.size() ) return new InvalidImp;
  else return new ConicImpPolar( calcConicBFFP( cs, type() ) );
}

ConicBFFPType::ConicBFFPType( const char* fullname, const ArgParser::spec* spec, int n )
  : ArgparserObjectType( fullname, spec, n )
{

}

ConicBFFPType::~ConicBFFPType()
{
}

static const char* constructellipsewithfocusstat =
  I18N_NOOP( "Construct an ellipse with this focus" );

static const ArgParser::spec argsspecEllipseBFFP[] =
{
  { ObjectImp::ID_PointImp, constructellipsewithfocusstat },
  { ObjectImp::ID_PointImp, constructellipsewithfocusstat },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct an ellipse through this point" ) }
};

EllipseBFFPType::EllipseBFFPType()
  : ConicBFFPType( "EllipseBFFP", argsspecEllipseBFFP, 3 )
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

static const char* constructhyperbolawithfocusstat =
  I18N_NOOP( "Construct a hyperbola with this focus" );

static const ArgParser::spec argsspecHyperbolaBFFP[] =
{
  { ObjectImp::ID_PointImp, constructhyperbolawithfocusstat },
  { ObjectImp::ID_PointImp, constructhyperbolawithfocusstat },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a hyperbola through this point" ) }
};

HyperbolaBFFPType::HyperbolaBFFPType()
  : ConicBFFPType( "HyperbolaBFFP", argsspecHyperbolaBFFP, 3 )
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
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a conic with this point as focus" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a conic through this point" ) },
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct a conic with this line as directrix" ) }
};

ConicBDFPType::ConicBDFPType()
  : ArgparserObjectType( "ConicBDFP", argsspeclpp, 3 )
{
}

ConicBDFPType::~ConicBDFPType()
{
}

ObjectImp* ConicBDFPType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;

  Args parsed = margsparser.parse( parents );

  if ( ! parsed[0] || ! parsed[2] || ( parents.size() == 3 && !parsed[1] ) )
    return new InvalidImp;

  assert( parsed[2]->inherits( ObjectImp::ID_LineImp ) );
  const LineData line = static_cast<const AbstractLineImp*>( parsed[2] )->data();

  assert( parsed[0]->inherits( ObjectImp::ID_PointImp ) );
  const Coordinate focus =
    static_cast<const PointImp*>( parsed[0] )->coordinate();

  Coordinate point;
  if ( parsed[1] )
  {
    assert( parsed[1]->inherits( ObjectImp::ID_PointImp ) );
    point = static_cast<const PointImp*>( parsed[1] )->coordinate();
  }
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

static const char* constructparabolathroughpointstat =
  I18N_NOOP( "Construct a parabola through this point" );

static const ArgParser::spec argsspecParabolaBTP[] =
{
  { ObjectImp::ID_PointImp, constructparabolathroughpointstat },
  { ObjectImp::ID_PointImp, constructparabolathroughpointstat },
  { ObjectImp::ID_PointImp, constructparabolathroughpointstat }
};

ParabolaBTPType::ParabolaBTPType()
  : ArgparserObjectType( "ParabolaBTP", argsspecParabolaBTP, 3 )
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

ObjectImp* ParabolaBTPType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );
  if ( points.size() != parents.size() ) return new InvalidImp;
  return new ConicImpCart(
    calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry ) );
}

static const ArgParser::spec argsspeccl[] =
{
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Construct a polar point wrt. this conic" ) },
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct the polar point of this line" ) }
};

ConicPolarPointType::ConicPolarPointType()
  : ArgparserObjectType( "ConicPolarPoint", argsspeccl, 2 )
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

ObjectImp* ConicPolarPointType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 2 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  if ( ! parsed[0] || ! parsed[1] )
    return new InvalidImp;
  const ConicCartesianData c = static_cast<const ConicImp*>( parsed[0] )->cartesianData();
  const LineData l = static_cast<const AbstractLineImp*>( parsed[1] )->data();
  bool valid = true;
  const Coordinate p = calcConicPolarPoint( c, l, valid );
  if ( valid ) return new PointImp( p );
  else return new InvalidImp;
}

static const ArgParser::spec argsspeccp[] =
{
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Construct a polar line wrt. this conic" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct the polar line of this point" ) }
};

ConicPolarLineType::ConicPolarLineType()
  : ArgparserObjectType( "ConicPolarLine", argsspeccp, 2 )
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

ObjectImp* ConicPolarLineType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 2 ) return new InvalidImp;
  Args parsed = margsparser.parse( parents );
  if ( !parsed[0] || ! parsed[1] )
    return new InvalidImp;
  const ConicCartesianData c = static_cast<const ConicImp*>( parsed[0] )->cartesianData();
  const Coordinate p = static_cast<const PointImp*>( parsed[1] )->coordinate();
  bool valid = true;
  const LineData l = calcConicPolarLine( c, p, valid );
  if ( valid ) return new LineImp( l );
  else return new InvalidImp;
}

static const ArgParser::spec argsspecc[] =
{
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Construct the directrix of this conic" ) }
};

ConicDirectrixType::ConicDirectrixType()
  : ArgparserObjectType( "ConicDirectrix", argsspecc, 1 )
{
}

ConicDirectrixType::~ConicDirectrixType()
{
}

const ConicDirectrixType* ConicDirectrixType::instance()
{
  static const ConicDirectrixType t;
  return &t;
}

ObjectImp* ConicDirectrixType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 1 ) return new InvalidImp;
  if ( ! parents[0]->valid() ) return new InvalidImp;
  assert( parents[0]->inherits( ObjectImp::ID_ConicImp ) );
  const ConicPolarData data =
    static_cast<const ConicImp*>( parents[0] )->polarData();

  double ec = data.ecostheta0;
  double es = data.esintheta0;
  double eccsq = ec*ec + es*es;

  Coordinate a = data.focus1 - data.pdimen/eccsq*Coordinate(ec,es);
  Coordinate b = a + Coordinate(-es,ec);
  return new LineImp( a, b );
}

static const char* hyperbolatpstatement = I18N_NOOP( "Construct a hyperbola through this point" );

static const ArgParser::spec argsspec4p[] =
{
  { ObjectImp::ID_PointImp, hyperbolatpstatement },
  { ObjectImp::ID_PointImp, hyperbolatpstatement },
  { ObjectImp::ID_PointImp, hyperbolatpstatement },
  { ObjectImp::ID_PointImp, hyperbolatpstatement }
};

EquilateralHyperbolaB4PType::EquilateralHyperbolaB4PType()
  : ArgparserObjectType( "EquilateralHyperbolaB4P", argsspec4p, 4 )
{
}

EquilateralHyperbolaB4PType::~EquilateralHyperbolaB4PType()
{
}

const EquilateralHyperbolaB4PType* EquilateralHyperbolaB4PType::instance()
{
  static const EquilateralHyperbolaB4PType t;
  return &t;
}

ObjectImp* EquilateralHyperbolaB4PType::calc( const Args& parents, const KigDocument& ) const
{
  std::vector<Coordinate> pts;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      pts.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  if ( parents.size() != pts.size() ) return new InvalidImp;
  return new ConicImpCart( calcConicThroughPoints( pts, equilateral ) );
}

static const ArgParser::spec argsspecParabolaBDP[] =
{
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct a parabola with this directrix" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a parabola through this point" ) }
};

ParabolaBDPType::ParabolaBDPType()
  : ObjectLPType( "ParabolaBDP", argsspecParabolaBDP, 2 )
{
}

ParabolaBDPType::~ParabolaBDPType()
{
}

const ParabolaBDPType* ParabolaBDPType::instance()
{
  static const ParabolaBDPType t;
  return &t;
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
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Construct the asymptotes of this conic" ) },
  { ObjectImp::ID_IntImp, "param" }
};

ConicAsymptoteType::ConicAsymptoteType()
  : ArgparserObjectType( "ConicAsymptote", argsspecci, 2 )
{
}

ConicAsymptoteType::~ConicAsymptoteType()
{
}

const ConicAsymptoteType* ConicAsymptoteType::instance()
{
  static const ConicAsymptoteType t;
  return &t;
}

ObjectImp* ConicAsymptoteType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 2 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( ! p[0] || ! p[1] )
    return new InvalidImp;
  bool valid = true;
  const LineData ret = calcConicAsymptote(
    static_cast<const ConicImp*>( p[0] )->cartesianData(),
    static_cast<const IntImp*>( p[1] )->data(),
    valid );
  if ( valid ) return new LineImp( ret );
  else return new InvalidImp;
}

static const char* radicallinesstatement = I18N_NOOP( "Construct the radical lines of this conic" );

static const ArgParser::spec argsspecccl[] =
{
  { ObjectImp::ID_ConicImp, radicallinesstatement },
  { ObjectImp::ID_ConicImp, radicallinesstatement },
  { ObjectImp::ID_IntImp, "param" },
  { ObjectImp::ID_IntImp, "param" }
};

ConicRadicalType::ConicRadicalType()
  : ArgparserObjectType( "ConicRadical", argsspecccl, 4 )
{
}

const ConicRadicalType* ConicRadicalType::instance()
{
  static const ConicRadicalType t;
  return &t;
}

ObjectImp* ConicRadicalType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 4 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( ! p[0] || ! p[1] || ! p[2] || ! p[3] )
    return new InvalidImp;
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

int ConicB5PType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int ConicBAAPType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int ConicBFFPType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int ConicBDFPType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int ParabolaBTPType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int EquilateralHyperbolaB4PType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int ConicPolarPointType::resultId() const
{
  return ObjectImp::ID_PointImp;
}

int ConicPolarLineType::resultId() const
{
  return ObjectImp::ID_LineImp;
}

int ConicDirectrixType::resultId() const
{
  return ObjectImp::ID_LineImp;
}

int ParabolaBDPType::resultId() const
{
  return ObjectImp::ID_ConicImp;
}

int ConicAsymptoteType::resultId() const
{
  return ObjectImp::ID_LineImp;
}

int ConicRadicalType::resultId() const
{
  return ObjectImp::ID_LineImp;
}

