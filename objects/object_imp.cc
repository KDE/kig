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

QString ObjectImp::translatedBaseName() const
{
  return i18n( baseName() );
}

const char* ObjectImp::idToString( int id )
{
  switch( id )
  {
  case ID_DoubleImp: return "double";
  case ID_IntImp: return "int";
  case ID_StringImp: return "string";
  case ID_PointImp: return "point";
  case ID_CurveImp: return "curve";
  case ID_LineImp: return "line";
  case ID_LabelImp: return "label";
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
  case ID_DoubleImp: return "Select this number";
  case ID_IntImp: return "Select this number";
  case ID_StringImp: return "Select this string";
  case ID_PointImp: return "Select this point";
  case ID_CurveImp: return "Select this curve";
  case ID_LineImp: return "Select this line";
  case ID_LabelImp: return "Select this label";
  case ID_AngleImp: return "Select this angle";
  case ID_VectorImp: return "Select this vector";
  case ID_LocusImp: return "Select this locus";
  case ID_CircleImp: return "Select this circle";
  case ID_ConicImp: return "Select this conic";
  case ID_CubicImp: return "Select this cubic";
  case ID_SegmentImp: return "Select this segment";
  case ID_RayImp: return "Select this ray";
  case ID_AnyImp: return "Select this object";
  default: return 0;
  };
}

int ObjectImp::stringToID( const QCString& string )
{
  if( string == "double" ) return ID_DoubleImp;
  if( string == "int" ) return ID_IntImp;
  if( string == "string" ) return ID_StringImp;
  if( string == "point" ) return ID_PointImp;
  if( string == "curve" ) return ID_CurveImp;
  if( string == "line" ) return ID_LineImp;
  if( string == "label" ) return ID_LabelImp;
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
