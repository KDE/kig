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

static const ArgsParser::spec argsspecTranslation[] =
{
  { ObjectImp::stype(), I18N_NOOP("Translate this object"), false },
  { VectorImp::stype(), I18N_NOOP("Translate by this vector"), false }
};

TranslatedType::TranslatedType()
  : ArgsParserObjectType( "Translation", argsspecTranslation, 2 )
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

ObjectImp* TranslatedType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  Coordinate dir = static_cast<const VectorImp*>( args[1] )->dir();
  Transformation t = Transformation::translation( dir );

  return args[0]->transform( t );
}

static const ArgsParser::spec argsspecPointReflection[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Reflect this object" ), false },
  { PointImp::stype(), I18N_NOOP( "Reflect around this point" ), false }
};

PointReflectionType::PointReflectionType()
  : ArgsParserObjectType( "PointReflection", argsspecPointReflection, 2 )
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

ObjectImp* PointReflectionType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  Coordinate center = static_cast<const PointImp*>( args[1] )->coordinate();
  Transformation t = Transformation::pointReflection( center );

  return args[0]->transform( t );
}

static const ArgsParser::spec argsspecLineReflection[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Reflect this object" ), false },
  { AbstractLineImp::stype(), I18N_NOOP( "Reflect over this line" ), false }
};

LineReflectionType::LineReflectionType()
  : ArgsParserObjectType( "LineReflection", argsspecLineReflection, 2 )
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

ObjectImp* LineReflectionType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  LineData d = static_cast<const AbstractLineImp*>( args[1] )->data();
  Transformation t = Transformation::lineReflection( d );

  return args[0]->transform( t );
}

static const ArgsParser::spec argsspecRotation[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Rotate this object" ), false },
  { PointImp::stype(), I18N_NOOP( "Rotate around this point" ), false },
  { AngleImp::stype(), I18N_NOOP( "Rotate by this angle" ), false }
};

RotationType::RotationType()
  : ArgsParserObjectType( "Rotation", argsspecRotation, 3 )
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

ObjectImp* RotationType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  Coordinate center = static_cast<const PointImp*>( args[1] )->coordinate();
  double angle = static_cast<const AngleImp*>( args[2] )->size();

  return args[0]->transform( Transformation::rotation( angle, center ) );
}

static const ArgsParser::spec argsspecScalingOverCenter[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Scale this object" ), false },
  { PointImp::stype(), I18N_NOOP( "Scale with this center" ), false },
  { SegmentImp::stype(), I18N_NOOP( "Scale by the length of this segment" ), false }
};

ScalingOverCenterType::ScalingOverCenterType()
  : ArgsParserObjectType( "ScalingOverCenter", argsspecScalingOverCenter, 3 )
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

ObjectImp* ScalingOverCenterType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  Coordinate center = static_cast<const PointImp*>( args[1] )->coordinate();
  double ratio = static_cast<const SegmentImp*>( args[2] )->length();

  return args[0]->transform( Transformation::scaling( ratio, center ) );
}

static const ArgsParser::spec argsspecScalingOverLine[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Scale this object" ), false },
  { AbstractLineImp::stype(), I18N_NOOP( "Scale over this line" ), false },
  { SegmentImp::stype(), I18N_NOOP( "Scale by the length of this segment" ), false }
};

ScalingOverLineType::ScalingOverLineType()
  : ArgsParserObjectType( "ScalingOverLine", argsspecScalingOverLine, 3 )
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

ObjectImp* ScalingOverLineType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  LineData line = static_cast<const AbstractLineImp*>( args[1] )->data();
  double ratio = static_cast<const SegmentImp*>( args[2] )->length();

  return args[0]->transform( Transformation::scaling( ratio, line ) );
}

static const ArgsParser::spec argsspecProjectiveRotation[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Projectively rotate this object" ), false },
  { RayImp::stype(), I18N_NOOP( "Projectively rotate with this ray" ), false },
  { AngleImp::stype(), I18N_NOOP( "Projectively rotate by this angle" ), false }
};

ProjectiveRotationType::ProjectiveRotationType()
  : ArgsParserObjectType( "ProjectiveRotation", argsspecProjectiveRotation, 3 )
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

ObjectImp* ProjectiveRotationType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const RayImp* ray = static_cast<const RayImp*>( args[1] );
  Coordinate c1 = ray->data().a;
  Coordinate dir = ray->data().dir().normalize();
  double alpha = static_cast<const AngleImp*>( args[2] )->size();

  return args[0]->transform(
    Transformation::projectiveRotation( alpha, dir, c1 ) );
}

static const ArgsParser::spec argsspecHarmonicHomology[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Harmonic Homology of this object" ), false },
  { PointImp::stype(), I18N_NOOP( "Harmonic Homology with this center" ), false },
  { AbstractLineImp::stype(),
    I18N_NOOP( "Harmonic Homology with this axis" ), false }
};

HarmonicHomologyType::HarmonicHomologyType()
  : ArgsParserObjectType( "HarmonicHomology", argsspecHarmonicHomology, 3 )
{
}

HarmonicHomologyType::~HarmonicHomologyType()
{
}

const HarmonicHomologyType* HarmonicHomologyType::instance()
{
  static const HarmonicHomologyType t;
  return &t;
}

ObjectImp* HarmonicHomologyType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  Coordinate center = static_cast<const PointImp*>( args[1] )->coordinate();
  LineData axis = static_cast<const AbstractLineImp*>( args[2] )->data();
  return args[0]->transform(
    Transformation::harmonicHomology( center, axis ) );
}

static const ArgsParser::spec argsspecAffinityGI3P[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Generic affinity of this object" ), false },
  { PointImp::stype(), I18N_NOOP( "First of 3 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Second of 3 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Third of 3 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of first point" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of second point" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of third point" ), false },
};

AffinityGI3PType::AffinityGI3PType()
  : ArgsParserObjectType( "AffinityGI3P", argsspecAffinityGI3P, 7 )
{
}

AffinityGI3PType::~AffinityGI3PType()
{
}

const AffinityGI3PType* AffinityGI3PType::instance()
{
  static const AffinityGI3PType t;
  return &t;
}

ObjectImp* AffinityGI3PType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  std::vector<Coordinate> frompoints;
  std::vector<Coordinate> topoints;
  for ( uint i = 0; i < 3; ++i )
  {
    frompoints.push_back(
           static_cast<const PointImp*>( args[i+1] )->coordinate() );
    topoints.push_back(
           static_cast<const PointImp*>( args[i+4] )->coordinate() );
  }

  bool valid = true;
  Transformation t = Transformation::affinityGI3P( frompoints, topoints,
        valid );

  if (valid == false) return new InvalidImp;
  return args[0]->transform( t );
}

static const ArgsParser::spec argsspecProjectivityGI4P[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Generic projectivity of this object" ), false },
  { PointImp::stype(), I18N_NOOP( "First of 4 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Second of 4 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Third of 4 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Fourth of 4 starting points" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of first point" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of second point" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of third point" ), false },
  { PointImp::stype(), I18N_NOOP( "Transformed position of fourth point" ), false }
};

ProjectivityGI4PType::ProjectivityGI4PType()
  : ArgsParserObjectType( "ProjectivityGI4P", argsspecProjectivityGI4P, 9 )
{
}

ProjectivityGI4PType::~ProjectivityGI4PType()
{
}

const ProjectivityGI4PType* ProjectivityGI4PType::instance()
{
  static const ProjectivityGI4PType t;
  return &t;
}

ObjectImp* ProjectivityGI4PType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  std::vector<Coordinate> frompoints;
  std::vector<Coordinate> topoints;
  for ( uint i = 0; i < 4; ++i )
  {
    frompoints.push_back(
           static_cast<const PointImp*>( args[i+1] )->coordinate() );
    topoints.push_back(
           static_cast<const PointImp*>( args[i+5] )->coordinate() );
  }

  bool valid = true;
  Transformation t = Transformation::projectivityGI4P( frompoints, topoints,
        valid );

  if (valid == false) return new InvalidImp;
  return args[0]->transform( t );
}

static const ArgsParser::spec argsspecCastShadow[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Cast the shadow of this object" ), false },
  { PointImp::stype(), I18N_NOOP( "Cast a shadow from this light source" ), false },
  { AbstractLineImp::stype(),
    I18N_NOOP( "Cast a shadow on the plane defined by this line" ), false }
};

CastShadowType::CastShadowType()
  : ArgsParserObjectType( "CastShadow", argsspecCastShadow, 3 )
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

ObjectImp* CastShadowType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  Coordinate lightsrc = static_cast<const PointImp*>( args[1] )->coordinate();
  LineData d = static_cast<const AbstractLineImp*>( args[2] )->data();
  return args[0]->transform(
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

const ObjectImpType* HarmonicHomologyType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* AffinityGI3PType::resultId() const
{
  return ObjectImp::stype();
}

const ObjectImpType* ProjectivityGI4PType::resultId() const
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

bool HarmonicHomologyType::isTransform() const
{
  return true;
}

bool AffinityGI3PType::isTransform() const
{
  return true;
}

bool ProjectivityGI4PType::isTransform() const
{
  return true;
}

bool CastShadowType::isTransform() const
{
  return true;
}

static const ArgsParser::spec argsspecApplyTransformation[] =
{
  { ObjectImp::stype(), I18N_NOOP( "Transform this object" ), false },
  { TransformationImp::stype(), I18N_NOOP( "Transform using this transformation" ), false }
};

ApplyTransformationObjectType::ApplyTransformationObjectType()
  : ArgsParserObjectType( "ApplyTransformation", argsspecApplyTransformation, 2 )
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

ObjectImp* ApplyTransformationObjectType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;
  return args[0]->transform( static_cast<const TransformationImp*>( args[1] )->data() );
}

const ObjectImpType* ApplyTransformationObjectType::resultId() const
{
  return ObjectImp::stype();
}

bool ApplyTransformationObjectType::isTransform() const
{
  return true;
}
