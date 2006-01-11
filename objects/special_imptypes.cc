// Copyright (C)  2005  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#include "special_imptypes.h"
#include "line_imp.h"
#include "other_imp.h"
#include "bogus_imp.h"
#include "text_imp.h"
#include "circle_imp.h"

static char snbs[] = "SHOULD NOT BE SEEN";

double getDoubleFromImp( const ObjectImp* obj, bool& valid )
{
  valid = true;

  if ( obj->inherits( SegmentImp::stype() ) )
    return static_cast<const SegmentImp*>( obj )->length();

  if ( obj->inherits( ArcImp::stype() ) )
  {
    const ArcImp* arc = static_cast<const ArcImp*>( obj );
    return arc->radius()*arc->angle();
  }

  if ( obj->inherits( AngleImp::stype() ) )
    return static_cast<const AngleImp*>( obj )->size();

  if ( obj->inherits( DoubleImp::stype() ) )
    return static_cast<const DoubleImp*>( obj )->data();

  if ( obj->inherits( NumericTextImp::stype() ) )
    return static_cast<const NumericTextImp*>( obj )->getValue();

  valid = false;
  return 0;
}

LengthImpType::LengthImpType( const ObjectImpType* parent,
    const char* internalname )
  : ObjectImpType( parent, internalname, snbs, snbs,
                   snbs, snbs, snbs, snbs, snbs, snbs, snbs )
{
}

LengthImpType::~LengthImpType()
{
}

bool LengthImpType::match( const ObjectImpType* t ) const
{
  return t == this || t == SegmentImp::stype() || t == ArcImp::stype()
                   || t == NumericTextImp::stype() || t == DoubleImp::stype();
}

LengthImpType lengthimptypeinstance(
    ObjectImp::stype(), "length-object" );

AngleImpType::AngleImpType( const ObjectImpType* parent,
    const char* internalname )
  : ObjectImpType( parent, internalname, snbs, snbs,
                   snbs, snbs, snbs, snbs, snbs, snbs, snbs )
{
}

AngleImpType::~AngleImpType()
{
}

bool AngleImpType::match( const ObjectImpType* t ) const
{
  return t == this || t == AngleImp::stype() || t == NumericTextImp::stype();
}

AngleImpType angleimptypeinstance(
    ObjectImp::stype(), "angle-object" );

InvertibleImpType::InvertibleImpType( const ObjectImpType* parent,
    const char* internalname )
  : ObjectImpType( parent, internalname, snbs, snbs,
                   snbs, snbs, snbs, snbs, snbs, snbs, snbs )
{
}

InvertibleImpType::~InvertibleImpType()
{
}

bool InvertibleImpType::match( const ObjectImpType* t ) const
{
  return t == this || t == LineImp::stype() || t == RayImp::stype() ||
         t == SegmentImp::stype() || t == CircleImp::stype() ||
         t == ArcImp::stype();
}

InvertibleImpType invertibleimptypeinstance(
    ObjectImp::stype(), "invertible-object" );
