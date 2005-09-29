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

const uint TextImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QCStringList TextImp::propertiesInternalNames() const
{
  QCStringList ret = Parent::propertiesInternalNames();
  ret << "kig_text";
  return ret;
}

const QCStringList TextImp::properties() const
{
  QCStringList ret = Parent::properties();
  ret << I18N_NOOP( "Text" );
  return ret;
}

const ObjectImpType* TextImp::impRequirementForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  return TextImp::stype();
}

const char* TextImp::iconForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "text"; // text
  else assert( false );
  return "";
}

ObjectImp* TextImp::property( uint which, const KigDocument& w ) const
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

bool TextImp::isPropertyDefinedOnOrThroughThisImp( uint which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

Rect TextImp::surroundingRect() const
{
  return mboundrect;
}
