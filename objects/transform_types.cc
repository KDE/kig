// transform_types.cc
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

#include "transform_types.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "line_imp.h"
#include "other_imp.h"
#include "../misc/coordinate.h"
#include "../misc/kigtransform.h"

#include <cmath>

static const ArgParser::spec argsspecTranslation[] =
{
  { VectorImp::stype(), I18N_NOOP("Translate by this vector") },
  { ObjectImp::stype(), I18N_NOOP("Translate this object") }
};

TranslatedType::TranslatedType()
  : ArgparserObjectType( "Translation", argsspecTranslation, 2 )
{
}

TranslatedType::~TranslatedType()
{
}

const TranslatedType* TranslatedType::instance()
{
  static const TranslatedType t;
  return &t;
}

ObjectImp* TranslatedType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 2 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if ( !args[0] || ! args[1] ) return new InvalidImp;
  assert( args[0]->inherits( VectorImp::stype() ) );

  Coordinate dir = static_cast<const VectorImp*>( args[0] )->dir();
  Transformation t = Transformation::translation( dir );
  return args[1]->transform( t );
}

static const ArgParser::spec argsspecPointReflection[] =
{
  { PointImp::stype(), I18N_NOOP( "Reflect around this point" ) },
  { ObjectImp::stype(), I18N_NOOP( "Reflect this object" ) }
};

PointReflectionType::PointReflectionType()
  : ArgparserObjectType( "PointReflection", argsspecPointReflection, 2 )
{
}

PointReflectionType::~PointReflectionType()
{
}

const PointReflectionType* PointReflectionType::instance()
{
  static const PointReflectionType t;
  return &t;
}

ObjectImp* PointReflectionType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 2 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if( !args[0] ) return new InvalidImp;

  assert( args[0]->inherits( PointImp::stype() ) );
  Coordinate center = static_cast<const PointImp*>( args[0] )->coordinate();
  Transformation t = Transformation::pointReflection( center );
  return args[1]->transform( t );
}

static const ArgParser::spec argsspecLineReflection[] =
{
  { AbstractLineImp::stype(), I18N_NOOP( "Reflect over this line" ) },
  { ObjectImp::stype(), I18N_NOOP( "Reflect this object" ) }
};

LineReflectionType::LineReflectionType()
  : ArgparserObjectType( "LineReflection", argsspecLineReflection, 2 )
{
}

LineReflectionType::~LineReflectionType()
{
}

const LineReflectionType* LineReflectionType::instance()
{
  static const LineReflectionType t;
  return &t;
}

ObjectImp* LineReflectionType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 2 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if( !args[0] ) return new InvalidImp;

  LineData d = static_cast<const AbstractLineImp*>( args[0] )->data();
  Transformation t = Transformation::lineReflection( d );
  return args[1]->transform( t );
}

static const ArgParser::spec argsspecRotation[] =
{
  { PointImp::stype(), I18N_NOOP( "Rotate around this point" ) },
  { AngleImp::stype(), I18N_NOOP( "Rotate by this angle" ) },
  { ObjectImp::stype(), I18N_NOOP( "Rotate this object" ) }
};

RotationType::RotationType()
  : ArgparserObjectType( "Rotation", argsspecRotation, 3 )
{
}

RotationType::~RotationType()
{
}

const RotationType* RotationType::instance()
{
  static const RotationType t;
  return &t;
}

ObjectImp* RotationType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 3 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if( !args[0] || ! args[1] ) return new InvalidImp;

  Coordinate center = static_cast<const PointImp*>( args[0] )->coordinate();
  double angle = static_cast<const AngleImp*>( args[1] )->size();

  return args[2]->transform( Transformation::rotation( angle, center ) );
}

static const ArgParser::spec argsspecScalingOverCenter[] =
{
  { PointImp::stype(), I18N_NOOP( "Scale with this center" ) },
  { SegmentImp::stype(), I18N_NOOP( "Scale by the length of this segment" ) },
  { ObjectImp::stype(), I18N_NOOP( "Scale this object" ) }
};

ScalingOverCenterType::ScalingOverCenterType()
  : ArgparserObjectType( "ScalingOverCenter", argsspecScalingOverCenter, 3 )
{
}

ScalingOverCenterType::~ScalingOverCenterType()
{
}

const ScalingOverCenterType* ScalingOverCenterType::instance()
{
  static const ScalingOverCenterType t;
  return &t;
}

ObjectImp* ScalingOverCenterType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 3 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if( !args[0] || ! args[1] ) return new InvalidImp;

  Coordinate center = static_cast<const PointImp*>( args[0] )->coordinate();
  double ratio = static_cast<const SegmentImp*>( args[1] )->length();

  return args[2]->transform( Transformation::scaling( ratio, center ) );
}

static const ArgParser::spec argsspecScalingOverLine[] =
{
  { SegmentImp::stype(), I18N_NOOP( "Scale by the length of this segment" ) },
  { AbstractLineImp::stype(), I18N_NOOP( "Scale over this line" ) },
  { ObjectImp::stype(), I18N_NOOP( "Scale this object" ) }
};

ScalingOverLineType::ScalingOverLineType()
  : ArgparserObjectType( "ScalingOverLine", argsspecScalingOverLine, 3 )
{
}

ScalingOverLineType::~ScalingOverLineType()
{
}

const ScalingOverLineType* ScalingOverLineType::instance()
{
  static const ScalingOverLineType t;
  return &t;
}

ObjectImp* ScalingOverLineType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 3 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if( !args[0] || ! args[1] ) return new InvalidImp;

  double ratio = static_cast<const SegmentImp*>( args[0] )->length();
  LineData line = static_cast<const AbstractLineImp*>( args[1] )->data();

  return args[2]->transform( Transformation::scaling( ratio, line ) );
}

static const ArgParser::spec argsspecProjectiveRotation[] =
{
  { RayImp::stype(), I18N_NOOP( "Projectively rotate with this ray" ) },
  { AngleImp::stype(), I18N_NOOP( "Projectively rotate by this angle" ) },
  { ObjectImp::stype(), I18N_NOOP( "Projectively rotate this object" ) }
};

ProjectiveRotationType::ProjectiveRotationType()
  : ArgparserObjectType( "ProjectiveRotation", argsspecProjectiveRotation, 3 )
{
}

ProjectiveRotationType::~ProjectiveRotationType()
{
}

const ProjectiveRotationType* ProjectiveRotationType::instance()
{
  static const ProjectiveRotationType t;
  return &t;
}

ObjectImp* ProjectiveRotationType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() < 2 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if ( !args[2] || !args[0] ) return new InvalidImp;
  assert( args[0]->inherits( RayImp::stype() ) );
  const RayImp* ray = static_cast<const RayImp*>( args[0] );
  Coordinate c1 = ray->data().a;
  Coordinate dir = ray->data().dir().normalize();
  double alpha = 0.1*M_PI/2;
  if ( args[1] )
  {
    assert( args[1]->inherits( AngleImp::stype() ) );
    alpha = static_cast<const AngleImp*>( args[1] )->size();
  };
  return args[2]->transform(
    Transformation::projectiveRotation( alpha, dir, c1 ) );
}

static const ArgParser::spec argsspecCastShadow[] =
{
  { PointImp::stype(), I18N_NOOP( "Cast a shadow from this light source" ) },
  { AbstractLineImp::stype(), I18N_NOOP( "Cast a shadow on the plane defined by this line" ) },
  { ObjectImp::stype(), I18N_NOOP( "Cast the shadow of this object" ) }
};

CastShadowType::CastShadowType()
  : ArgparserObjectType( "CastShadow", argsspecCastShadow, 3 )
{
}

CastShadowType::~CastShadowType()
{
}

const CastShadowType* CastShadowType::instance()
{
  static const CastShadowType t;
  return &t;
}

ObjectImp* CastShadowType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() != 3 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if( !args[0] || ! args[1] || ! args[2] ) return new InvalidImp;
  Coordinate lightsrc = static_cast<const PointImp*>( args[0] )->coordinate();
  LineData d = static_cast<const AbstractLineImp*>( args[1] )->data();
  return args[2]->transform(
    Transformation::castShadow( lightsrc, d ) );
}

const ObjectImpType* TranslatedType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* PointReflectionType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* LineReflectionType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* RotationType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* ScalingOverCenterType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* ScalingOverLineType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* ProjectiveRotationType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* CastShadowType::resultId() const
{
  return ObjectImp::stype();
}

bool TranslatedType::isTransform() const
{
  return true;
}

bool PointReflectionType::isTransform() const
{
  return true;
}

bool LineReflectionType::isTransform() const
{
  return true;
}

bool RotationType::isTransform() const
{
  return true;
}

bool ScalingOverCenterType::isTransform() const
{
  return true;
}

bool ScalingOverLineType::isTransform() const
{
  return true;
}

bool ProjectiveRotationType::isTransform() const
{
  return true;
}

bool CastShadowType::isTransform() const
{
  return true;
}

static const ArgParser::spec argsspecApplyTransformation[] =
{
  { TransformationImp::stype(), I18N_NOOP( "Transform using this transformation" ) },
  { ObjectImp::stype(), I18N_NOOP( "Transform this object" ) }
};

ApplyTransformationObjectType::ApplyTransformationObjectType()
  : ArgparserObjectType( "ApplyTransformation", argsspecApplyTransformation, 2 )
{
}

ApplyTransformationObjectType::~ApplyTransformationObjectType()
{
}

const ApplyTransformationObjectType* ApplyTransformationObjectType::instance()
{
  static const ApplyTransformationObjectType t;
  return &t;
}

ObjectImp* ApplyTransformationObjectType::calc( const Args& targs, const KigDocument& ) const
{
  assert( targs.size() == 2 );
  const Args& args = margsparser.parse( targs );
  assert( args[0] && args[1] );
  assert( args[0]->inherits( TransformationImp::stype() ) );
  return args[1]->transform( static_cast<const TransformationImp*>( args[0] )->data() );
}

const ObjectImpType* ApplyTransformationObjectType::resultId() const
{
  return ObjectImp::stype();
}

bool ApplyTransformationObjectType::isTransform() const
{
  return true;
}
