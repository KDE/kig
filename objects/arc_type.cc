// SPDX-FileCopyrightText: 2003-2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "arc_type.h"

#include "bogus_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "conic_imp.h"
#include "line_imp.h"
#include "locus_imp.h"

#include "../misc/common.h"
#include "../misc/calcpaths.h"
#include "../misc/goniometry.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"

#include <functional>
#include <algorithm>
#include <cmath>
#include <math.h>

using std::find;

#include <QStringList>

/*
 * oriented arc by three points
 */

static const char constructarcstartingstat[] = I18N_NOOP( "Construct an arc starting at this point" );

static const ArgsParser::spec argsspecArcBTP[] =
{
  { PointImp::stype(), constructarcstartingstat,
    I18N_NOOP( "Select the start point of the new arc..." ), true },
  { PointImp::stype(), I18N_NOOP( "Construct an arc through this point" ),
    I18N_NOOP( "Select a point for the new arc to go through..." ), true },
  { PointImp::stype(), I18N_NOOP( "Construct an arc ending at this point" ),
    I18N_NOOP( "Select the end point of the new arc..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ArcBTPType )

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
  if ( ! margsparser.checkArgs( args, 2 ) )
    return new InvalidImp;

  const Coordinate a =
    static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate b =
    static_cast<const PointImp*>( args[1] )->coordinate();
  Coordinate center;
  double angle = 0.;
  double startangle = 0.;
  int orientation = 1;
  if ( args.size() == 3 )
  {
    Coordinate c = static_cast<const PointImp*>( args[2] )->coordinate();
    center = calcCenter( a, b, c );
    if ( ! center.valid() )
    {
/* TODO: return correctly oriented segment! */
      if ( fabs( a.x - c.x ) > fabs( a.y - c.y ) )
      {
        if ( ( b.x - a.x)*(c.x - b.x) > 1e-12 ) return new SegmentImp(a, c);
      } else
      {
        if ( ( b.y - a.y)*(c.y - b.y) > 1e-12 ) return new SegmentImp(a, c);
      }
      return new InvalidImp;
    }
    /* this is also done in calcCenter... should optimize in some way */
    double xdo = b.x-a.x;
    double ydo = b.y-a.y;

    double xao = c.x-a.x;
    double yao = c.y-a.y;

    if ( xdo * yao - xao * ydo < 0.0 ) orientation = -1;

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
    center = (b+a)/2 + .6*(b-a).orthogonal();
    Coordinate bd = b - center;
    Coordinate ad = a - center;
    startangle = atan2( ad.y, ad.x );
    double halfangle = atan2( bd.y, bd.x ) - startangle;
    if ( halfangle < - M_PI ) halfangle += 2*M_PI;
    angle = 2 * halfangle;
  };

  double radius = ( a - center ).length();
  return new ArcImp( center, orientation*radius, startangle, angle );
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

/*
 * arc by center, starting point and angle
 */

static const ArgsParser::spec argsspecArcBCPA[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct an arc with this center" ),
    I18N_NOOP( "Select the center of the new arc..." ), true },
  { PointImp::stype(), constructarcstartingstat,
    I18N_NOOP( "Select the start point of the new arc..." ), true },
  { AngleImp::stype(), I18N_NOOP( "Construct an arc with this angle" ),
    I18N_NOOP( "Select the angle of the new arc..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ArcBCPAType )

ArcBCPAType::ArcBCPAType()
  : ArgsParserObjectType( "ArcBCPA", argsspecArcBCPA, 3 )
{
}

ArcBCPAType::~ArcBCPAType()
{
}

const ArcBCPAType* ArcBCPAType::instance()
{
  static const ArcBCPAType t;
  return &t;
}

ObjectImp* ArcBCPAType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) )
    return new InvalidImp;

  const Coordinate center = static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate p = static_cast<const PointImp*>( args[1] )->coordinate();
  const AngleImp* a = static_cast<const AngleImp*>( args[2] );
  const double angle = a->angle();
  const Coordinate dir = p - center;
  const double startangle = atan2( dir.y, dir.x );
  const double radius = center.distance( p );

  return new ArcImp( center, radius, startangle, angle );
}

const ObjectImpType* ArcBCPAType::impRequirement( const ObjectImp*, const Args& ) const
{
  return PointImp::stype();
}

bool ArcBCPAType::inherits( int type ) const
{
  return Parent::inherits( type );
}

const ObjectImpType* ArcBCPAType::resultId() const
{
  return ArcImp::stype();
}

/*
 * arc of conic by three points and center
 */

static const char constructconicarcstartingstat[] = I18N_NOOP( "Construct a conic arc starting at this point" );
static const char selectconicarcstartingstat[] = I18N_NOOP( "Select the start point of the new conic arc..." );
static const char constructconicarcthrustat[] = I18N_NOOP( "Construct a conic arc through this point" );
static const char selectconicarcthrustat[] = I18N_NOOP( "Select a point for the new conic arc to go through..." );
static const char constructconicarcendingstat[] = I18N_NOOP( "Construct a conic arc ending at this point" );
static const char selectconicarcendingstat[] = I18N_NOOP( "Select the end point of the new conic arc..." );

static const ArgsParser::spec argsspecConicArcBCTP[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct an conic arc with this center" ),
    I18N_NOOP( "Select the center of the new conic arc..." ), false },
  { PointImp::stype(), constructconicarcstartingstat,
    selectconicarcstartingstat, true },
  { PointImp::stype(), constructconicarcthrustat,
    selectconicarcthrustat, true },
  { PointImp::stype(), constructconicarcendingstat,
    selectconicarcendingstat, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConicArcBCTPType )

ConicArcBCTPType::ConicArcBCTPType()
  : ArgsParserObjectType( "ConicArcBCTP", argsspecConicArcBCTP, 4 )
{
}

ConicArcBCTPType::~ConicArcBCTPType()
{
}

const ConicArcBCTPType* ConicArcBCTPType::instance()
{
  static const ConicArcBCTPType t;
  return &t;
}

ObjectImp* ConicArcBCTPType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args, 2 ) )
    return new InvalidImp;

  const Coordinate center =
    static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate a =
    static_cast<const PointImp*>( args[1] )->coordinate();
  const Coordinate d = 2*center - a;
  Coordinate b = center + (a-center).orthogonal();
  Coordinate e = 2*center - b;
  if ( args.size() >= 3 )
  {
    b = static_cast<const PointImp*>( args[2] )->coordinate();
    e = 2*center - b;
  }
  bool have_c = false;
  Coordinate c;
  if ( args.size() == 4 )
  {
    c = static_cast<const PointImp*>( args[3] )->coordinate();
    const Coordinate e = 2*center - c;
    have_c = true;
  }

  std::vector<Coordinate> points;
  points.push_back( a );
  points.push_back( b );
  if (have_c) points.push_back( c );
  points.push_back( d );
  points.push_back( e );
  ConicCartesianData cart =
    calcConicThroughPoints( points, zerotilt, circleifzt, ysymmetry );
  if ( ! d.valid() )
    return new InvalidImp;

  ConicArcImp *me = new ConicArcImp( cart, 0.0, 2*M_PI );
  double angle = 0.;
  double startangle = 0.;
  double anglea = 2*M_PI*me->getParam( a );
  double angleb = anglea + M_PI/2;
  angleb = 2*M_PI*me->getParam( b );
  double anglec = 2*angleb - anglea;
  if ( have_c ) anglec = 2*M_PI*me->getParam( c );

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

  me->setStartAngle( startangle );
  me->setAngle( angle );
  return me;
}

const ObjectImpType* ConicArcBCTPType::resultId() const
{
  return ConicArcImp::stype();
}

/*
 * arc of conic by five points
 */

static const ArgsParser::spec argsspecConicArcB5P[] =
{
  { PointImp::stype(), constructconicarcstartingstat,
    selectconicarcstartingstat, true },
  { PointImp::stype(), constructconicarcthrustat,
    selectconicarcthrustat, true },
  { PointImp::stype(), constructconicarcthrustat,
    selectconicarcthrustat, true },
  { PointImp::stype(), constructconicarcthrustat,
    selectconicarcthrustat, true },
  { PointImp::stype(), constructconicarcendingstat,
    selectconicarcendingstat, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConicArcB5PType )

ConicArcB5PType::ConicArcB5PType()
  : ArgsParserObjectType( "ConicArcB5P", argsspecConicArcB5P, 5 )
{
}

ConicArcB5PType::~ConicArcB5PType()
{
}

const ConicArcB5PType* ConicArcB5PType::instance()
{
  static const ConicArcB5PType t;
  return &t;
}

ObjectImp* ConicArcB5PType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args, 2 ) )
    return new InvalidImp;

  const Coordinate a =
    static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate b =
    static_cast<const PointImp*>( args[1] )->coordinate();

  Coordinate c, d, e;
  bool have_c = false;
  bool have_d = false;
  bool have_e = false;
  if ( args.size() >= 3 )
  {
    c = static_cast<const PointImp*>( args[2] )->coordinate();
    have_c = true;
  }
  if ( args.size() >= 4 )
  {
    d = static_cast<const PointImp*>( args[3] )->coordinate();
    have_d = true;
  }
  if ( args.size() >= 5 )
  {
    e = static_cast<const PointImp*>( args[4] )->coordinate();
    have_e = true;
  }

  std::vector<Coordinate> points;
  points.push_back( a );
  points.push_back( b );
  if (have_c) points.push_back( c );
  if (have_d) points.push_back( d );
  if (have_e) points.push_back( e );
  ConicCartesianData cart =
    calcConicThroughPoints( points, zerotilt, circleifzt, ysymmetry );
  if ( ! d.valid() )
    return new InvalidImp;

  ConicArcImp *me = new ConicArcImp( cart, 0.0, 2*M_PI );
  double angle = 0.;
  double startangle = 0.;
  double anglea = 2*M_PI*me->getParam( a );
  double angleb = anglea + M_PI/2;
  angleb = 2*M_PI*me->getParam( b );
  if ( have_c ) angleb = 2*M_PI*me->getParam( c );
  double anglec = 2*angleb - anglea;
  if ( have_e ) anglec = 2*M_PI*me->getParam( e );

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

  me->setStartAngle( startangle );
  me->setAngle( angle );
  return me;
}

const ObjectImpType* ConicArcB5PType::resultId() const
{
  return ConicArcImp::stype();
}

