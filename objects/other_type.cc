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

static const ArgsParser::spec argsspecAngle[] =
{
  { PointImp::stype(), constructanglethroughpoint },
  { PointImp::stype(), I18N_NOOP( "Construct an angle at this point" ) },
  { PointImp::stype(), constructanglethroughpoint }
};

AngleType::AngleType()
  : ArgsParserObjectType( "Angle", argsspecAngle, 3 )
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
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( uint i = 0; i < parents.size(); ++i )
    points.push_back(
      static_cast<const PointImp*>( parents[i] )->coordinate() );

  Coordinate lvect = points[0] - points[1];
  Coordinate rvect = points[2] - points[1];

  double startangle = atan2( lvect.y, lvect.x );
  double endangle = atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2* M_PI;
  if ( startangle < 0 ) startangle += 2*M_PI;

  return new AngleImp( points[1], startangle, anglelength );
}

static const ArgsParser::spec argsspecVector[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct a vector from this point" ) },
  { PointImp::stype(), I18N_NOOP( "Construct a vector to this point" ) }
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

static const struct ArgsParser::spec argsspecLocus[] =
{
  { HierarchyImp::stype(), "hierarchy" },
  { CurveImp::stype(), "curve" }
};

LocusType::LocusType()
  : ArgsParserObjectType( "Locus", argsspecLocus, 2 )
{
}

LocusType::~LocusType()
{
}

ObjectImp* LocusType::calc( const Args& args, const KigDocument& ) const
{
  using namespace std;

  assert( args.size() >= 2 );
  const Args firsttwo( args.begin(), args.begin() + 2 );
  Args fixedargs( args.begin() + 2, args.end() );

  if ( ! margsparser.checkArgs( firsttwo ) ) return new InvalidImp;
  for ( Args::iterator i = fixedargs.begin(); i != fixedargs.end(); ++i )
    if ( ! (*i)->valid() )
      return new InvalidImp;

  const ObjectHierarchy& hier =
    static_cast<const HierarchyImp*>( args[0] )->data();
  const CurveImp* curveimp = static_cast<const CurveImp*>( args[1] );

  return new LocusImp( curveimp->copy(), hier.withFixedArgs( fixedargs ) );
}

bool LocusType::inherits( int type ) const
{
  return type == ID_LocusType ? true : Parent::inherits( type );
}

const ObjectImpType* AngleType::resultId() const
{
  return AngleImp::stype();
}

const ObjectImpType* VectorType::resultId() const
{
  return VectorImp::stype();
}

const ObjectImpType* LocusType::resultId() const
{
  return LocusImp::stype();
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

const ObjectImpType* CopyObjectType::impRequirement( const ObjectImp*, const Args& ) const
{
  return ObjectImp::stype();
}

const ObjectImpType* CopyObjectType::resultId() const
{
  // we don't know what we return..
  return ObjectImp::stype();
}

const ObjectImpType* LocusType::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  assert( parents.size() >= 2 );
  Args firsttwo( parents.begin(), parents.begin() + 2 );
  if ( o == parents[0] || o == parents[1] )
    return margsparser.impRequirement( o, firsttwo );
  else
  {
    const HierarchyImp* h = dynamic_cast<const HierarchyImp*>( parents[0] );
    if (  h )
    {
      PointImp* p = new PointImp( Coordinate() );
      Args hargs( parents.begin()+ 2, parents.end() );
      hargs.push_back( p );
      ArgsParser hparser = h->data().argParser();
      const ObjectImpType* ret = hparser.impRequirement( o, hargs );
      delete p;
      return ret;
    }
    else
      return ObjectImp::stype();
  };
}

const LocusType* LocusType::instance()
{
  static const LocusType t;
  return &t;
}

static const ArgsParser::spec argsspecArcBTP[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct an arc starting at this point" ) },
  { PointImp::stype(), I18N_NOOP( "Construct an arc through this point" ) },
  { PointImp::stype(), I18N_NOOP( "Construct an arc ending at this point" ) }
};

ArcBTPType::ArcBTPType()
  : ArgsParserObjectType( "ArcBTP", argsspecArcBTP, 3 )
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
  if ( ! margsparser.checkArgs( args ) )
    return new InvalidImp;

  const Coordinate a =
    static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate b =
    static_cast<const PointImp*>( args[1] )->coordinate();
  Coordinate center;
  double angle = 0.;
  double startangle = 0.;
  if ( args.size() == 3 )
  {
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

const ObjectImpType* ArcBTPType::impRequirement( const ObjectImp*, const Args& ) const
{
  return PointImp::stype();
}

bool ArcBTPType::inherits( int type ) const
{
  return Parent::inherits( type );
}

const ObjectImpType* ArcBTPType::resultId() const
{
  return ArcImp::stype();
}

QStringList AngleType::specialActions() const
{
  QStringList ret;
  ret << i18n( "Set Si&ze" );
  return ret;
}

void AngleType::executeAction(
  int i, ObjectHolder&, ObjectTypeCalcer& t,
  KigDocument& d, KigWidget& w, NormalMode& ) const
{
  assert( i == 0 );
  // pretend to use this var..
  (void) i;

  std::vector<ObjectCalcer*> parents = t.parents();

  assert( margsparser.checkArgs( parents ) );

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
  parents[2]->move( nc, d );
  KigCommand* kc = new KigCommand( d, i18n( "Resize Angle" ) );
  mon.finish( kc );
  d.history()->addCommand( kc );
}

std::vector<ObjectCalcer*> CopyObjectType::sortArgs( const std::vector<ObjectCalcer*>& os ) const
{
  assert( os.size() == 1 );
  return os;
}

std::vector<ObjectCalcer*> LocusType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  assert( args.size() >= 2 );
  std::vector<ObjectCalcer*> firsttwo( args.begin(), args.begin() + 2 );
  firsttwo = margsparser.parse( firsttwo );
  std::copy( args.begin() + 2, args.end(), std::back_inserter( firsttwo ) );
  return firsttwo;
}

Args LocusType::sortArgs( const Args& args ) const
{
  assert( args.size() >= 2 );
  Args firsttwo( args.begin(), args.begin() + 2 );
  firsttwo = margsparser.parse( firsttwo );
  std::copy( args.begin() + 2, args.end(), std::back_inserter( firsttwo ) );
  return firsttwo;
}

Args CopyObjectType::sortArgs( const Args& args ) const
{
  assert( args.size() == 1 );
  return args;
}

