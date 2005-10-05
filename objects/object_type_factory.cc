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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "object_type_factory.h"

#include <config.h>

#include "object_type.h"
#include "circle_type.h"
#include "conic_types.h"
#include "cubic_type.h"
#include "intersection_types.h"
#include "line_type.h"
#include "text_type.h"
#include "other_type.h"
#include "transform_types.h"
#include "point_type.h"
#include "tests_type.h"

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/python_type.h"
#endif

#include <qdom.h>
#include <string>

ObjectTypeFactory::ObjectTypeFactory()
  : malreadysetup( false )
{
  setupBuiltinTypes();
}

ObjectTypeFactory::~ObjectTypeFactory()
{
}

ObjectTypeFactory* ObjectTypeFactory::instance()
{
  static ObjectTypeFactory fact;
  return &fact;
}

void ObjectTypeFactory::add( const ObjectType* type )
{
  assert( mmap.find( std::string( type->fullName() ) ) == mmap.end() );
  mmap[std::string( type->fullName() )] = type;
}

const ObjectType* ObjectTypeFactory::find( const char* name ) const
{
  maptype::const_iterator i = mmap.find( std::string( name ) );
  if ( i == mmap.end() ) return 0;
  else return i->second;
}

void ObjectTypeFactory::setupBuiltinTypes()
{
//   assert( ! malreadysetup );
//   malreadysetup = true;

//   // circle_type.h
//   add( CircleBCPType::instance() );
//   add( CircleBPRType::instance() );
//   add( CircleBTPType::instance() );

//   // conic_types.h
//   add( ConicB5PType::instance() );
//   add( ConicBAAPType::instance() );
//   add( EllipseBFFPType::instance() );
//   add( HyperbolaBFFPType::instance() );
//   add( ConicBDFPType::instance() );
//   add( ParabolaBTPType::instance() );
//   add( EquilateralHyperbolaB4PType::instance() );
//   add( ConicPolarPointType::instance() );
//   add( ConicPolarLineType::instance() );
//   add( ConicDirectrixType::instance() );
//   add( ParabolaBDPType::instance() );
//   add( ConicAsymptoteType::instance() );
//   add( ConicRadicalType::instance() );

//   // cubic_type.h
//   add( CubicB9PType::instance() );
//   add( CubicNodeB6PType::instance() );
//   add( CubicCuspB4PType::instance() );

//   // intersection_types.h
//   add( ConicLineIntersectionType::instance() );
//   add( ConicLineOtherIntersectionType::instance() );
//   add( LineLineIntersectionType::instance() );
//   add( LineCubicIntersectionType::instance() );
//   add( CircleCircleIntersectionType::instance() );

//   // line_type.h
//   add( SegmentABType::instance() );
//   add( LineABType::instance() );
//   add( RayABType::instance() );
//   add( LinePerpendLPType::instance() );
//   add( LineParallelLPType::instance() );

//   // other_type.h
//   add( AngleType::instance() );
//   add( VectorType::instance() );
//   add( LocusType::instance() );
//   add( ArcBTPType::instance() );
//   add( CopyObjectType::instance() );

//   // point_type.h
//   add( FixedPointType::instance() );
//   add( ConstrainedPointType::instance() );
//   add( MidPointType::instance() );
//   add( MeasureTransportType::instance() );

//   // text_type.h
//   add( TextType::instance() );

//   // tests_type.h
//   add( AreParallelType::instance() );

//   // transform_types.h
//   add( TranslatedType::instance() );
//   add( PointReflectionType::instance() );
//   add( LineReflectionType::instance() );
//   add( RotationType::instance() );
//   add( ScalingOverCenterType::instance() );
//   add( ScalingOverLineType::instance() );
//   add( ProjectiveRotationType::instance() );
//   add( CastShadowType::instance() );

// #ifdef KIG_ENABLE_PYTHON_SCRIPTING
//   // python types
//   add( PythonCompileType::instance() );
//   add( PythonExecuteType::instance() );
// #endif
}
