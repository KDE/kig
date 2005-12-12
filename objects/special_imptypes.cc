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

double getDoubleFromImp( const ObjectImp* obj, bool& valid )
{
  valid = true;

  if ( obj->inherits( SegmentImp::stype() ) )
    return static_cast<const SegmentImp*>( obj )->length();

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
    const char* internalname,
    const char* translatedname,
    const char* selectstatement,
    const char* selectnamestatement,
    const char* removeastatement,
    const char* addastatement,
    const char* moveastatement,
    const char* attachtothisstatement,
    const char* showastatement,
    const char* hideastatement )
  : ObjectImpType( parent, internalname, translatedname, selectstatement,
                   selectnamestatement, removeastatement, addastatement,
                   moveastatement, attachtothisstatement,
                   showastatement, hideastatement )
{
}

LengthImpType::~LengthImpType()
{
}

bool LengthImpType::match( const ObjectImpType* t ) const
{
  return t == this || t == SegmentImp::stype() || t == NumericTextImp::stype()
                   || t == DoubleImp::stype();
}

LengthImpType lengthimptypeinstance(
    ObjectImp::stype(), "length-object",
    I18N_NOOP( "length" ),
    I18N_NOOP( "Select this length" ),
    I18N_NOOP( "Select length %1" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" )
    );

AngleImpType::AngleImpType( const ObjectImpType* parent,
    const char* internalname,
    const char* translatedname,
    const char* selectstatement,
    const char* selectnamestatement,
    const char* removeastatement,
    const char* addastatement,
    const char* moveastatement,
    const char* attachtothisstatement,
    const char* showastatement,
    const char* hideastatement )
  : ObjectImpType( parent, internalname, translatedname, selectstatement,
                   selectnamestatement, removeastatement, addastatement,
                   moveastatement, attachtothisstatement,
                   showastatement, hideastatement )
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
    ObjectImp::stype(), "angle-object",
    I18N_NOOP( "angle" ),
    I18N_NOOP( "Select this angle" ),
    I18N_NOOP( "Select angle %1" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" ),
    I18N_NOOP( "SHOULD NOT BE SEEN" )
    );

