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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "point_imp.h"

#include "bogus_imp.h"
#include "../misc/kigtransform.h"
#include "../misc/kigpainter.h"
#include "../misc/coordinate_system.h"
#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <klocale.h>

PointImp::PointImp( const Coordinate& c )
  : mc( c )
{
}

Coordinate PointImp::attachPoint() const
{
  return mc;
//  return Coordinate::invalidCoord();
}

void PointImp::draw( KigPainter& p ) const
{
  p.drawFatPoint( mc );
}

bool PointImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  int twidth = width == -1 ? 5 : width;
  return (p - mc).length() - twidth*w.screenInfo().pixelWidth() < 0;
}

bool PointImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  double am = w.screenInfo().normalMiss( width );
  return r.contains( mc, am );
}

const uint PointImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 3;
}

const QCStringList PointImp::propertiesInternalNames() const
{
  QCStringList l = Parent::propertiesInternalNames();
  l << "coordinate";
  l << "coordinate-x";
  l << "coordinate-y";
  assert( l.size() == PointImp::numberOfProperties() );
  return l;
}

const QCStringList PointImp::properties() const
{
  QCStringList l = Parent::properties();
  l << I18N_NOOP( "Coordinate" );
  l << I18N_NOOP( "X coordinate" );
  l << I18N_NOOP( "Y coordinate" );
  assert( l.size() == PointImp::numberOfProperties() );
  return l;
}

const ObjectImpType* PointImp::impRequirementForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return PointImp::stype();
}

const char* PointImp::iconForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  if ( which == Parent::numberOfProperties() )
    return "pointxy"; // coordinate
  if ( which == Parent::numberOfProperties() + 1 )
    return "pointxy"; // coordinate-x
  if ( which == Parent::numberOfProperties() + 2 )
    return "pointxy"; // coordinate-y
  else assert( false );
  return "";
}

ObjectImp* PointImp::property( uint which, const KigDocument& d ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, d );
  if ( which == Parent::numberOfProperties() )
    return new PointImp( mc );
  if ( which == Parent::numberOfProperties() + 1 )
    return new DoubleImp( mc.x );
  if ( which == Parent::numberOfProperties() + 2 )
    return new DoubleImp( mc.y );
//  else assert( false );
  return new InvalidImp;
}

PointImp::~PointImp()
{
}

PointImp* PointImp::copy() const
{
  return new PointImp( mc );
}

ObjectImp* PointImp::transform( const Transformation& t ) const
{
  Coordinate nc = t.apply( mc );
  if ( nc.valid() ) return new PointImp( nc );
  else return new InvalidImp();
}

void PointImp::setCoordinate( const Coordinate& c )
{
  mc = c;
}

void PointImp::fillInNextEscape( QString& s, const KigDocument& doc ) const
{
  s = s.arg( doc.coordinateSystem().fromScreen( mc, doc ) );
}

void PointImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool PointImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( PointImp::stype() ) &&
    static_cast<const PointImp&>( rhs ).coordinate() == coordinate();
}

bool PointImp::canFillInNextEscape() const
{
  return true;
}

const ObjectImpType* PointImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "point",
    I18N_NOOP( "point" ),
    I18N_NOOP( "Select this point" ),
    I18N_NOOP( "Select point %1" ),
    I18N_NOOP( "Remove a Point" ),
    I18N_NOOP( "Add a Point" ),
    I18N_NOOP( "Move a Point" ),
    I18N_NOOP( "Attach to this point" ),
    I18N_NOOP( "Show a Point" ),
    I18N_NOOP( "Hide a Point" )
    );
  return &t;
}

const ObjectImpType* PointImp::type() const
{
  return PointImp::stype();
}

bool PointImp::isPropertyDefinedOnOrThroughThisImp( uint which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

Rect PointImp::surroundingRect() const
{
  return Rect( mc, 0., 0. );
}

/*
 */

BogusPointImp::BogusPointImp( const Coordinate& c )
  : PointImp( c )
{
}

BogusPointImp::~BogusPointImp()
{
}

const ObjectImpType* BogusPointImp::stype()
{
  static const ObjectImpType t(
    0, "boguspoint",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN",
    "SHOULDNOTBESEEN"
    );
  return &t;
}

const ObjectImpType* BogusPointImp::type() const
{
  return BogusPointImp::stype();
}
