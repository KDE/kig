// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>
// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "angle_type.h"

#include "bogus_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "../misc/calcpaths.h"
#include "../misc/common.h"
#include "../misc/goniometry.h"
#include "../misc/kiginputdialog.h"
#include "../kig/kig_commands.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include <functional>
#include <algorithm>
#include <cmath>
#include <math.h>

#include <QStringList>

static const char* constructanglethroughpoint =
  I18N_NOOP( "Construct an angle through this point" );

static const ArgsParser::spec argsspecAngle[] =
{
  { PointImp::stype(), constructanglethroughpoint,
    I18N_NOOP( "Select a point that the first half-line of the angle should go through..." ), true },
  { PointImp::stype(), I18N_NOOP( "Construct an angle at this point" ),
    I18N_NOOP( "Select the point to construct the angle in..." ), true },
  { PointImp::stype(), constructanglethroughpoint,
    I18N_NOOP( "Select a point that the second half-line of the angle should go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( AngleType )

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
  if ( ! margsparser.checkArgs( parents, 2 ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( uint i = 0; i < parents.size(); ++i )
    points.push_back(
      static_cast<const PointImp*>( parents[i] )->coordinate() );

  Coordinate lvect = points[0] - points[1];
  Coordinate rvect;
  bool markRightAngle = true;
  
  if ( points.size() == 3 )
  {
    rvect = points[2] - points[1];
  }
  else
  {
    rvect = lvect.orthogonal();
    markRightAngle = false; // angle is still incomplete
  }

  double startangle = atan2( lvect.y, lvect.x );
  double endangle = atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2* M_PI;
  if ( startangle < 0 ) startangle += 2*M_PI;

  return new AngleImp( points[1], startangle, anglelength, markRightAngle );
}

const ObjectImpType* AngleType::resultId() const
{
  return AngleImp::stype();
}

QStringList AngleType::specialActions() const
{
  QStringList ret;

  ret << i18n( "Set Si&ze" );
  ret << i18n( "Toggle &Right Angle Mark" );

  return ret;
}

void AngleType::executeAction(
  int i, ObjectHolder&, ObjectTypeCalcer& t,
  KigPart& d, KigWidget& w, NormalMode& ) const
{
  if ( i == 0 )
  {
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

    Goniometry go( anglelength, Goniometry::Rad );
    go.convertTo( Goniometry::Deg );

    bool ok;
    Goniometry newsize = KigInputDialog::getAngle( &w, &ok, go );
    if ( !ok )
      return;
    newsize.convertTo( Goniometry::Rad );

    double newcangle = startangle + newsize.value();
    Coordinate cdir( cos( newcangle ), sin( newcangle ) );
    Coordinate nc = b + cdir.normalize( rvect.length() );

    MonitorDataObjects mon( getAllParents( parents ) );
    parents[2]->move( nc, d.document() );
    KigCommand* kc = new KigCommand( d, i18n( "Resize Angle" ) );
    mon.finish( kc );
    d.history()->push( kc );
  }
  else if ( i == 1 )
  {
    AngleImp * angleImp = const_cast< AngleImp * >( dynamic_cast< const AngleImp *>( t.imp() ) );

    angleImp->setMarkRightAngle( !angleImp->markRightAngle() );
    d.redrawScreen();
  }
}

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( HalfAngleType )

HalfAngleType::HalfAngleType()
  : ArgsParserObjectType( "HalfAngle", argsspecAngle, 3 )
{
}

HalfAngleType::~HalfAngleType()
{
}

const HalfAngleType* HalfAngleType::instance()
{
  static const HalfAngleType t;
  return &t;
}

ObjectImp* HalfAngleType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 2 ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( uint i = 0; i < parents.size(); ++i )
    points.push_back(
      static_cast<const PointImp*>( parents[i] )->coordinate() );

  Coordinate lvect = points[0] - points[1];
  Coordinate rvect;
  if ( points.size() == 3 )
    rvect = points[2] - points[1];
  else
  {
    rvect = lvect.orthogonal();
  }

  double startangle = atan2( lvect.y, lvect.x );
  double endangle = atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2 * M_PI;
  if ( startangle < 0 ) startangle += 2 * M_PI;
  
  if ( anglelength > M_PI )
  {
    startangle += anglelength;
    anglelength = 2 * M_PI - anglelength;
    if ( startangle > 2 * M_PI ) startangle -= 2 * M_PI;
    if ( anglelength < 0 ) anglelength += 2 * M_PI;
  }

  return new AngleImp( points[1], startangle, anglelength, true );
}

const ObjectImpType* HalfAngleType::resultId() const
{
  return AngleImp::stype();
}

