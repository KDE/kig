// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "text_imp.h"

#include "bogus_imp.h"
#include "../misc/kigpainter.h"

TextImp::TextImp( const QString& text, const Coordinate& loc, bool frame )
  : mtext( text), mloc( loc ), mframe( frame ), mboundrect( Rect::invalidRect() )
{
}

TextImp* TextImp::copy() const
{
  return new TextImp( mtext, mloc );
}

TextImp::~TextImp()
{
}

Coordinate TextImp::attachPoint() const
{
  return Coordinate::invalidCoord();
}

ObjectImp* TextImp::transform( const Transformation& t ) const
{
  Coordinate nloc = t.apply( mloc );
  return new TextImp( mtext, nloc, mframe );
}

void TextImp::draw( KigPainter& p ) const
{
  mboundrect = p.simpleBoundingRect( mloc, mtext );
  p.drawTextFrame( mboundrect, mtext, mframe );
}

bool TextImp::contains( const Coordinate& p, int, const KigWidget& ) const
{
  return mboundrect.contains( p );
}

bool TextImp::inRect( const Rect& r, int, const KigWidget& ) const
{
  return mboundrect.intersects( r );
}

bool TextImp::valid() const
{
  return true;
}

int TextImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QByteArrayList TextImp::propertiesInternalNames() const
{
  QByteArrayList ret = Parent::propertiesInternalNames();
  ret << "kig_text";
  return ret;
}

const QByteArrayList TextImp::properties() const
{
  QByteArrayList ret = Parent::properties();
  ret << I18N_NOOP( "Text" );
  return ret;
}

const ObjectImpType* TextImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  return TextImp::stype();
}

const char* TextImp::iconForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "draw-text"; // text
  else assert( false );
  return "";
}

ObjectImp* TextImp::property( int which, const KigDocument& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
    return new StringImp( text() );
  else assert( false );
  return new InvalidImp;
}

QString TextImp::text() const
{
  return mtext;
}

void TextImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

const Coordinate TextImp::coordinate() const
{
  return mloc;
}

bool TextImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( TextImp::stype() ) &&
    static_cast<const TextImp&>( rhs ).coordinate() == coordinate() &&
    static_cast<const TextImp&>( rhs ).text() == text() &&
    static_cast<const TextImp&>( rhs ).hasFrame() == hasFrame();
}

bool TextImp::hasFrame() const
{
  return mframe;
}

const ObjectImpType* TextImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "label",
    I18N_NOOP( "label" ),
    I18N_NOOP( "Select this label" ),
    I18N_NOOP( "Select label %1" ),
    I18N_NOOP( "Remove a Label" ),
    I18N_NOOP( "Add a Label" ),
    I18N_NOOP( "Move a Label" ),
    I18N_NOOP( "Attach to this label" ),
    I18N_NOOP( "Show a Label" ),
    I18N_NOOP( "Hide a Label" )
    );
  return &t;
}

const ObjectImpType* TextImp::type() const
{
  return TextImp::stype();
}

bool TextImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

Rect TextImp::surroundingRect() const
{
  return mboundrect;
}

/*
 * NumericTextImp
 */

NumericTextImp::NumericTextImp( const QString& text, const Coordinate& loc, bool frame, double value )
  : TextImp( text, loc, frame ), mvalue( value )
{
}

NumericTextImp* NumericTextImp::copy() const
{
  return new NumericTextImp( text(), coordinate(), hasFrame(), mvalue );
}

const ObjectImpType* NumericTextImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "numeric-label",
    I18N_NOOP( "numeric label" ),
    I18N_NOOP( "Select this numeric label" ),
    I18N_NOOP( "Select numeric label %1" ),
    I18N_NOOP( "Remove a Numeric Label" ),
    I18N_NOOP( "Add a Numeric Label" ),
    I18N_NOOP( "Move a Numeric Label" ),
    I18N_NOOP( "Attach to this numeric label" ),
    I18N_NOOP( "Show a Numeric Label" ),
    I18N_NOOP( "Hide a Numeric Label" )
    );
  return &t;
}

const ObjectImpType* NumericTextImp::type() const
{
  return NumericTextImp::stype();
}

double NumericTextImp::getValue( void ) const
{
  return mvalue;
}

int NumericTextImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QByteArrayList NumericTextImp::propertiesInternalNames() const
{
  QByteArrayList ret = Parent::propertiesInternalNames();
  ret << "kig_value";
  return ret;
}

const QByteArrayList NumericTextImp::properties() const
{
  QByteArrayList ret = Parent::properties();
  ret << I18N_NOOP( "Numeric value" );
  return ret;
}

const ObjectImpType* NumericTextImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  return NumericTextImp::stype();
}

const char* NumericTextImp::iconForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "value"; // text
  else assert( false );
  return "";
}

ObjectImp* NumericTextImp::property( int which, const KigDocument& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
    return new DoubleImp( getValue() );
  else assert( false );
  return new InvalidImp;
}

bool NumericTextImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

/*
 * BoolTextImp
 */

BoolTextImp::BoolTextImp( const QString& text, const Coordinate& loc, bool frame, bool value )
  : TextImp( text, loc, frame ), mvalue( value )
{
}

BoolTextImp* BoolTextImp::copy() const
{
  return new BoolTextImp( text(), coordinate(), hasFrame(), mvalue );
}

const ObjectImpType* BoolTextImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "boolean-label",
    I18N_NOOP( "boolean label" ),
    I18N_NOOP( "Select this boolean label" ),
    I18N_NOOP( "Select boolean label %1" ),
    I18N_NOOP( "Remove a Boolean Label" ),
    I18N_NOOP( "Add a Boolean Label" ),
    I18N_NOOP( "Move a Boolean Label" ),
    I18N_NOOP( "Attach to this boolean label" ),
    I18N_NOOP( "Show a Boolean Label" ),
    I18N_NOOP( "Hide a Boolean Label" )
    );
  return &t;
}

const ObjectImpType* BoolTextImp::type() const
{
  return BoolTextImp::stype();
}

bool BoolTextImp::getValue( void ) const
{
  return mvalue;
}

int BoolTextImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QByteArrayList BoolTextImp::propertiesInternalNames() const
{
  QByteArrayList ret = Parent::propertiesInternalNames();
  ret << "kig_value";
  return ret;
}

const QByteArrayList BoolTextImp::properties() const
{
  QByteArrayList ret = Parent::properties();
  ret << I18N_NOOP( "Numeric value" );
  return ret;
}

const ObjectImpType* BoolTextImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  return NumericTextImp::stype();
}

const char* BoolTextImp::iconForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "value"; // text
  else assert( false );
  return "";
}

ObjectImp* BoolTextImp::property( int which, const KigDocument& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
    return new DoubleImp( getValue() );
  else assert( false );
  return new InvalidImp;
}

bool BoolTextImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}
