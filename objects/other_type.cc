// other_type.cc
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

#include "other_type.h"

#include "bogus_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "locus_imp.h"
#include "object.h"

#include "../misc/common.h"
#include "../misc/calcpaths.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"

#include <functional>
#include <algorithm>
#include <cmath>

using std::find;

#include <qstringlist.h>

static const char* constructanglethroughpoint =
  I18N_NOOP( "Construct an angle through this point" );

static const ArgParser::spec argsspecAngle[] =
{
  { ObjectImp::ID_PointImp, constructanglethroughpoint },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct an angle at this point" ) },
  { ObjectImp::ID_PointImp, constructanglethroughpoint }
};

AngleType::AngleType()
  : ArgparserObjectType( "Angle", argsspecAngle, 3 )
{
}

AngleType::~AngleType()
{
}

const AngleType* AngleType::instance()
{
  static const AngleType t;
  return &t;
}

ObjectImp* AngleType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 3 ) return new InvalidImp;
  std::vector<Coordinate> points;
  for ( uint i = 0; i < parents.size(); ++i )
    if ( parents[i]->inherits( ObjectImp::ID_PointImp ) )
      points.push_back(
        static_cast<const PointImp*>( parents[i] )->coordinate() );
  if ( points.size() != parents.size() )
    return new InvalidImp;

  Coordinate lvect = points[0] - points[1];
  Coordinate rvect = points[2] - points[1];

  double startangle = atan2( lvect.y, lvect.x );
  double endangle = atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2* M_PI;
  if ( startangle < 0 ) startangle += 2*M_PI;

  return new AngleImp( points[1], startangle, anglelength );
}

static const ArgParser::spec argsspecVector[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a vector from this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a vector to this point" ) }
};

VectorType::VectorType()
  : ObjectABType( "Vector", argsspecVector, 2 )
{
}

VectorType::~VectorType()
{
}

const VectorType* VectorType::instance()
{
  static const VectorType t;
  return &t;
}

ObjectImp* VectorType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new VectorImp( a, b );
}

static const struct ArgParser::spec argsspecLocus[] =
{
  { ObjectImp::ID_HierarchyImp, "hierarchy" },
  { ObjectImp::ID_CurveImp, "curve" }
};

LocusType::LocusType()
  : ArgparserObjectType( "Locus", argsspecLocus, 2 )
{
}

LocusType::~LocusType()
{
}

ObjectImp* LocusType::calc( const Args& targs, const KigDocument& ) const
{
  using namespace std;

  if ( targs.size() < 2 ) return new InvalidImp;
  const Args firsttwo( targs.begin(), targs.begin() + 2 );
  const Args args = margsparser.parse( firsttwo );
  if ( ! args[0] || !args[1] ) return new InvalidImp;
  const ObjectHierarchy& hier =
    static_cast<const HierarchyImp*>( args[0] )->data();
  const CurveImp* curveimp = static_cast<const CurveImp*>( args[1] );

  Args fixedargs( targs.begin() + 2, targs.end() );

  return new LocusImp( curveimp->copy(), hier.withFixedArgs( fixedargs ) );
}

bool LocusType::inherits( int type ) const
{
  return type == ID_LocusType ? true : Parent::inherits( type );
}

int AngleType::resultId() const
{
  return ObjectImp::ID_AngleImp;
}

int VectorType::resultId() const
{
  return ObjectImp::ID_VectorImp;
}

int LocusType::resultId() const
{
  return ObjectImp::ID_LocusImp;
}

CopyObjectType::CopyObjectType()
  : ObjectType( "Copy" )
{
}

CopyObjectType::~CopyObjectType()
{
}

CopyObjectType* CopyObjectType::instance()
{
  static CopyObjectType t;
  return &t;
}

bool CopyObjectType::inherits( int ) const
{
  return false;
}

ObjectImp* CopyObjectType::calc( const Args& parents, const KigDocument& ) const
{
  assert( parents.size() == 1 );
  return parents[0]->copy();
}

int CopyObjectType::impRequirement( const ObjectImp*, const Args& ) const
{
  return ObjectImp::ID_AnyImp;
}

int CopyObjectType::resultId() const
{
  // we don't know what we return..
  return ObjectImp::ID_AnyImp;
}

int LocusType::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  Args firsttwo( parents.begin(), parents.begin() + 2 );
  if ( find( firsttwo.begin(), firsttwo.end(), o ) != firsttwo.end() )
    return margsparser.impRequirement( o, firsttwo );
  else
    return ObjectImp::ID_AnyImp;
}

const LocusType* LocusType::instance()
{
  static const LocusType t;
  return &t;
}

static const ArgParser::spec argsspecArcBTP[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct an arc starting at this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct an arc through this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct an arc ending at this point" ) }
};

ArcBTPType::ArcBTPType()
  : ArgparserObjectType( "ArcBTP", argsspecArcBTP, 3 )
{
}

ArcBTPType::~ArcBTPType()
{
}

const ArcBTPType* ArcBTPType::instance()
{
  static const ArcBTPType t;
  return &t;
}

ObjectImp* ArcBTPType::calc( const Args& args, const KigDocument& ) const
{
  if ( args.size() < 2 ) return new InvalidImp;
  if ( args.size() > 3 ) return new InvalidImp;
  assert( args[0]->inherits( ObjectImp::ID_PointImp ) );
  assert( args[1]->inherits( ObjectImp::ID_PointImp ) );
  const Coordinate a =
    static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate b =
    static_cast<const PointImp*>( args[1] )->coordinate();
  Coordinate center;
  double angle = 0.;
  double startangle = 0.;
  if ( args.size() == 3 )
  {
    assert( args[2]->inherits( ObjectImp::ID_PointImp ) );
    Coordinate c = static_cast<const PointImp*>( args[2] )->coordinate();
    center = calcCenter( a, b, c );
    if ( ! center.valid() ) return new InvalidImp;
    Coordinate ad = a - center;
    Coordinate bd = b - center;
    Coordinate cd = c - center;
    double anglea = atan2( ad.y, ad.x );
    double angleb = atan2( bd.y, bd.x );
    double anglec = atan2( cd.y, cd.x );

    // anglea should be smaller than anglec
    if ( anglea > anglec )
    {
      double t = anglea;
      anglea = anglec;
      anglec = t;
    };
    if ( angleb > anglec || angleb < anglea )
    {
      startangle = anglec;
      angle = 2 * M_PI + anglea - startangle;
    }
    else
    {
      startangle = anglea;
      angle = anglec - anglea;
    };
  }
  else
  {
    // find a center and angles that look natural..
    center = (b+a)/2 + 2 * (b-a).orthogonal();
    Coordinate bd = b - center;
    Coordinate ad = a - center;
    startangle = atan2( ad.y, ad.x );
    double halfangle = atan2( bd.y, bd.x ) - startangle;
    if ( halfangle < - M_PI ) halfangle += 2*M_PI;
    angle = 2 * halfangle;
  };

  double radius = ( a - center ).length();
  return new ArcImp( center, radius, startangle, angle );
}

int ArcBTPType::impRequirement( const ObjectImp*, const Args& ) const
{
  return ObjectImp::ID_PointImp;
}

bool ArcBTPType::inherits( int type ) const
{
  return Parent::inherits( type );
}

int ArcBTPType::resultId() const
{
  return ObjectImp::ID_ArcImp;
}

QStringList AngleType::specialActions() const
{
  QStringList ret;
  ret << i18n( "Set Si&ze" );
  return ret;
}

void AngleType::executeAction(
  int i, RealObject* o, KigDocument& d, KigWidget& w,
  NormalMode& ) const
{
  assert( i == 0 );

  Objects parents = o->parents();
  assert( parents.size() == 3 );

  if ( ! parents[0]->hasimp( ObjectImp::ID_PointImp ) ||
       ! parents[1]->hasimp( ObjectImp::ID_PointImp ) ||
       ! parents[2]->hasimp( ObjectImp::ID_PointImp ) )
    return;

  Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  Coordinate c = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();

  Coordinate lvect = a - b;
  Coordinate rvect = c - b;

  double startangle = atan2( lvect.y, lvect.x );
  double endangle = atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2* M_PI;
  if ( startangle < 0 ) startangle += 2*M_PI;

  int anglelengthdeg = static_cast<int>( anglelength * 180 / M_PI );

  bool ok = true;
  double newsize = getDoubleFromUser(
    i18n( "Set Angle Size" ), i18n( "Choose the new size: " ),
    anglelengthdeg, &w, &ok, -2147483647, 2147483647, 0 );
  if ( ! ok ) return;

  newsize *= M_PI;
  newsize /= 180;

  double newcangle = startangle + newsize;
  Coordinate cdir( cos( newcangle ), sin( newcangle ) );
  Coordinate nc = b + cdir.normalize( rvect.length() );

  MonitorDataObjects mon( getAllParents( parents ) );
  parents[2]->move( c, nc - c, d );
  KigCommand* kc = new KigCommand( d, i18n( "Resize &Angle" ) );
  kc->addTask( mon.finish() );
  d.history()->addCommand( kc );
}
