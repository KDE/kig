// object_type_factory.cc
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

#include "object_type_factory.h"

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
#include "custom_types.h"

#include <qdom.h>

#include <string>

ObjectTypeFactory::ObjectTypeFactory()
  : malreadysetup( false )
{
  setupBuiltinTypes();
}

ObjectTypeFactory::~ObjectTypeFactory()
{
  for ( maptype::iterator i = mmap.begin(); i != mmap.end(); ++i )
    delete i->second;
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
  assert( ! malreadysetup );
  malreadysetup = true;

  // circle_type.h
  add( CircleBCPType::instance() );
  add( CircleBPRType::instance() );
  add( CircleBTPType::instance() );

  // conic_types.h
  add( ConicB5PType::instance() );
  add( ConicBAAPType::instance() );
  add( EllipseBFFPType::instance() );
  add( HyperbolaBFFPType::instance() );
  add( ConicBDFPType::instance() );
  add( ParabolaBTPType::instance() );
  add( EquilateralHyperbolaB4PType::instance() );
  add( ConicPolarPointType::instance() );
  add( ConicPolarLineType::instance() );
  add( ConicDirectrixType::instance() );
  add( ParabolaBDPType::instance() );
  add( ConicAsymptoteType::instance() );
  add( ConicRadicalType::instance() );

  // cubic_type.h
  add( CubicB9PType::instance() );
  add( CubicNodeB6PType::instance() );
  add( CubicCuspB4PType::instance() );

  // intersection_types.h
  add( ConicLineIntersectionType::instance() );
  add( LineLineIntersectionType::instance() );
  add( LineCubicIntersectionType::instance() );

  // line_type.h
  add( SegmentABType::instance() );
  add( LineABType::instance() );
  add( RayABType::instance() );
  add( LinePerpendLPType::instance() );
  add( LineParallelLPType::instance() );

  // other_type.h
  add( AngleType::instance() );
  add( VectorType::instance() );

  // point_type.h
  add( FixedPointType::instance() );
  add( ConstrainedPointType::instance() );
  add( MidPointType::instance() );

  // text_type.h
  add( TextType::instance() );

  // transform_types.h
  add( TranslatedType::instance() );
  add( PointReflectionType::instance() );
  add( LineReflectionType::instance() );
  add( RotationType::instance() );
  add( ScalingOverCenterType::instance() );
  add( ScalingOverLineType::instance() );
  add( ProjectiveRotationType::instance() );
  add( CastShadowType::instance() );
}

QString ObjectTypeFactory::serialize( const CustomType& t,
                                      QDomElement& parent,
                                      QDomDocument& doc )
{
  if ( t.inherits( ObjectType::ID_LocusType ) )
  {
    const LocusType& type = static_cast<const LocusType&>( t );

    QDomElement hierelem = doc.createElement( "Hierarchy" );
    type.hierarchy().serialize( hierelem, doc );

    parent.appendChild( hierelem );

    return QString::fromLatin1( "Locus" );
  }
//   else if ( t.inherits( ObjectType::ID_MacroType ) )
//   {
//   }
  else assert( false );
}

CustomType* ObjectTypeFactory::deserialize( const QString& type, QDomElement& parent )
{
  CustomType* ret = 0;
  if ( type == "Locus" )
  {
    QDomElement hierelem = parent.firstChild().toElement();
    if ( hierelem.isNull() || hierelem.tagName() != "Hierarchy" ) ret = 0;
    else {
      ObjectHierarchy hier( hierelem );
      ret = new LocusType( hier );
    };
  }
  if ( ! ret ) return 0;
  CustomTypes::instance()->add( ret );
  return ret;
}
