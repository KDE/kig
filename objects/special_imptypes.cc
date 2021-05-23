// SPDX-FileCopyrightText: 2005 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

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

WeightImpType::WeightImpType( const ObjectImpType* parent,
    const char* internalname )
  : ObjectImpType( parent, internalname, snbs, snbs,
                   snbs, snbs, snbs, snbs, snbs, snbs, snbs )
{
}

WeightImpType::~WeightImpType()
{
}

bool WeightImpType::match( const ObjectImpType* t ) const
{
  return t == this || t == SegmentImp::stype() || t == ArcImp::stype()
                   || t == NumericTextImp::stype() || t == DoubleImp::stype();
}

WeightImpType weightimptypeinstance(
    ObjectImp::stype(), "weight-object" );

InvertibleImpType::InvertibleImpType( const ObjectImpType* parent,
    const char* internalname )
  : ObjectImpType( parent, internalname, snbs, I18N_NOOP( "Select this object" ),
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
