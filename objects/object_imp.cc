// object_imp.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "object_imp.h"

#include "bogus_imp.h"

#include "../misc/coordinate.h"

#include <klocale.h>

ObjectImp::ObjectImp()
{
}

ObjectImp::~ObjectImp()
{
}

bool ObjectImp::valid() const
{
  return true;
}

bool ObjectImp::inherits( int type ) const
{
  return type == ID_AnyImp;
}

QString ObjectImp::translatedName( int id )
{
  switch( id )
  {
  case ID_DoubleImp: return i18n( "double" );
  case ID_IntImp: return i18n( "int" );
  case ID_StringImp: return i18n( "string" );
  case ID_HierarchyImp: return i18n( "hierarchy" );
  case ID_PointImp: return i18n( "point" );
  case ID_CurveImp: return i18n( "curve" );
  case ID_LineImp: return i18n( "line" );
  case ID_TextImp: return i18n( "label" );
  case ID_AngleImp: return i18n( "angle" );
  case ID_VectorImp: return i18n( "vector" );
  case ID_LocusImp: return i18n( "locus" );
  case ID_CircleImp: return i18n( "circle" );
  case ID_ConicImp: return i18n( "conic" );
  case ID_CubicImp: return i18n( "cubic" );
  case ID_SegmentImp: return i18n( "segment" );
  case ID_RayImp: return i18n( "ray" );
//  case ID_AnyImp: return "any";
  default: return QString::null;
  }
}

const char* ObjectImp::idToString( int id )
{
  switch( id )
  {
  case ID_DoubleImp: return "double";
  case ID_IntImp: return "int";
  case ID_StringImp: return "string";
  case ID_HierarchyImp: return "hierarchy";
  case ID_PointImp: return "point";
  case ID_CurveImp: return "curve";
  case ID_LineImp: return "line";
  case ID_TextImp: return "label";
  case ID_AngleImp: return "angle";
  case ID_VectorImp: return "vector";
  case ID_LocusImp: return "locus";
  case ID_CircleImp: return "circle";
  case ID_ConicImp: return "conic";
  case ID_CubicImp: return "cubic";
  case ID_SegmentImp: return "segment";
  case ID_RayImp: return "ray";
  case ID_AnyImp: return "any";
  default: return 0;
  }
}

const char* ObjectImp::selectStatement( int id )
{
  switch( id )
  {
  case ID_DoubleImp: return I18N_NOOP( "Select this number" );
  case ID_IntImp: return I18N_NOOP( "Select this number" );
  case ID_StringImp: return I18N_NOOP( "Select this string" );
  case ID_HierarchyImp: return I18N_NOOP( "Select this hierarchy" );
  case ID_PointImp: return I18N_NOOP( "Select this point" );
  case ID_CurveImp: return I18N_NOOP( "Select this curve" );
  case ID_LineImp: return I18N_NOOP( "Select this line" );
  case ID_TextImp: return I18N_NOOP( "Select this label" );
  case ID_AngleImp: return I18N_NOOP( "Select this angle" );
  case ID_VectorImp: return I18N_NOOP( "Select this vector" );
  case ID_LocusImp: return I18N_NOOP( "Select this locus" );
  case ID_CircleImp: return I18N_NOOP( "Select this circle" );
  case ID_ConicImp: return I18N_NOOP( "Select this conic" );
  case ID_CubicImp: return I18N_NOOP( "Select this cubic" );
  case ID_SegmentImp: return I18N_NOOP( "Select this segment" );
  case ID_RayImp: return I18N_NOOP( "Select this ray" );
  case ID_AnyImp: return I18N_NOOP( "Select this object" );
  default: return 0;
  };
}

int ObjectImp::stringToID( const QCString& string )
{
  if( string == "double" ) return ID_DoubleImp;
  if( string == "int" ) return ID_IntImp;
  if( string == "string" ) return ID_StringImp;
  if( string == "hierarchy" ) return ID_HierarchyImp;
  if( string == "point" ) return ID_PointImp;
  if( string == "curve" ) return ID_CurveImp;
  if( string == "line" ) return ID_LineImp;
  if( string == "label" ) return ID_TextImp;
  if( string == "angle" ) return ID_AngleImp;
  if( string == "vector" ) return ID_VectorImp;
  if( string == "locus" ) return ID_LocusImp;
  if( string == "circle" ) return ID_CircleImp;
  if( string == "conic" ) return ID_ConicImp;
  if( string == "cubic" ) return ID_CubicImp;
  if( string == "segment" ) return ID_SegmentImp;
  if( string == "ray" ) return ID_RayImp;
  if( string == "any" ) return ID_AnyImp;
  return -1;
}

QString ObjectImp::removeAStatement( int id )
{
  switch( id )
  {
  case ID_DoubleImp: return i18n( "Remove a number" );
  case ID_IntImp: return i18n( "Remove a number" );
  case ID_StringImp: return i18n( "Remove a string" );
  case ID_HierarchyImp: return i18n( "Remove a hierarchy" );
  case ID_PointImp: return i18n( "Remove a point" );
  case ID_CurveImp: return i18n( "Remove a curve" );
  case ID_LineImp: return i18n( "Remove a line" );
  case ID_TextImp: return i18n( "Remove a label" );
  case ID_AngleImp: return i18n( "Remove an angle" );
  case ID_VectorImp: return i18n( "Remove a vector" );
  case ID_LocusImp: return i18n( "Remove a locus" );
  case ID_CircleImp: return i18n( "Remove a circle" );
  case ID_ConicImp: return i18n( "Remove a conic" );
  case ID_CubicImp: return i18n( "Remove a cubic" );
  case ID_SegmentImp: return i18n( "Remove a segment" );
  case ID_RayImp: return i18n( "Remove a ray" );
  case ID_AnyImp: return i18n( "Remove an object" );
  default: return 0;
  };
}

QString ObjectImp::addAStatement( int id )
{
  switch( id )
  {
  case ID_DoubleImp: return i18n( "Add a number" );
  case ID_IntImp: return i18n( "Add a number" );
  case ID_StringImp: return i18n( "Add a string" );
  case ID_HierarchyImp: return i18n( "Add a hierarchy" );
  case ID_PointImp: return i18n( "Add a point" );
  case ID_CurveImp: return i18n( "Add a curve" );
  case ID_LineImp: return i18n( "Add a line" );
  case ID_TextImp: return i18n( "Add a label" );
  case ID_AngleImp: return i18n( "Add an angle" );
  case ID_VectorImp: return i18n( "Add a vector" );
  case ID_LocusImp: return i18n( "Add a locus" );
  case ID_CircleImp: return i18n( "Add a circle" );
  case ID_ConicImp: return i18n( "Add a conic" );
  case ID_CubicImp: return i18n( "Add a cubic" );
  case ID_SegmentImp: return i18n( "Add a segment" );
  case ID_RayImp: return i18n( "Add a ray" );
  case ID_AnyImp: return i18n( "Add an object" );
  default: return 0;
  };
}

void ObjectImp::fillInNextEscape( QString&, const KigDocument& ) const
{
  assert( false );
}

const QCStringList ObjectImp::properties() const
{
  QCStringList ret;
  ret << I18N_NOOP( "Base Object Type" );
  return ret;
}

const uint ObjectImp::numberOfProperties() const
{
  return 1;
}

const QCStringList ObjectImp::propertiesInternalNames() const
{
  QCStringList ret;
  ret << "base-object-type";
  return ret;
}

ObjectImp* ObjectImp::property( uint i, const KigDocument& ) const
{
  if ( i == 0 ) return new StringImp( translatedName( id() ) );
  return new InvalidImp;
}

int ObjectImp::impRequirementForProperty( uint ) const
{
  return ID_AnyImp;
}

void ObjectImpVisitor::visit( const ObjectImp* imp )
{
  imp->visit( this );
}

void ObjectImpVisitor::visit( const IntImp* )
{
}

void ObjectImpVisitor::visit( const DoubleImp* )
{
}

void ObjectImpVisitor::visit( const StringImp* )
{
}

void ObjectImpVisitor::visit( const InvalidImp* )
{
}

void ObjectImpVisitor::visit( const HierarchyImp* )
{
}

void ObjectImpVisitor::visit( const LineImp* )
{
}

void ObjectImpVisitor::visit( const PointImp* )
{
}

void ObjectImpVisitor::visit( const TextImp* )
{
}

void ObjectImpVisitor::visit( const AngleImp* )
{
}

void ObjectImpVisitor::visit( const VectorImp* )
{
}

void ObjectImpVisitor::visit( const LocusImp* )
{
}

void ObjectImpVisitor::visit( const CircleImp* )
{
}

void ObjectImpVisitor::visit( const ConicImp* )
{
}

void ObjectImpVisitor::visit( const CubicImp* )
{
}

void ObjectImpVisitor::visit( const SegmentImp* )
{
}

void ObjectImpVisitor::visit( const RayImp* )
{
}

void ObjectImpVisitor::visit( const ArcImp* )
{
}

ObjectImpVisitor::~ObjectImpVisitor()
{

}
