/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "macro.h"

#include "../misc/hierarchy.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate.h"
#include "../misc/type.h"
#include "../misc/i18n.h"

MacroObject::MacroObject( ObjectHierarchy* inHier, const Objects& args )
  : hier(inHier), arguments( args )
{
  assert( args.size() == inHier->getGegElems().size() );
  std::for_each( arguments.begin(), arguments.end(),
                 std::bind2nd(
                     std::mem_fun( &Object::addChild ), this ) );
}

MacroObjectOne::MacroObjectOne( const MType* type, ObjectHierarchy* inHier, const Objects& args )
  : MacroObject( inHier, args ), final( 0 ), mtype( type )
{
  assert (inHier->getFinElems().size() == 1);
  construct();
}

MacroObjectOne::~MacroObjectOne()
{
  delete_all( cos.begin(), cos.end() );
  delete final;
}

void MacroObjectOne::draw(KigPainter& p, bool ss) const
{
  final->setSelected( selected );
  final->setShown( shown() );
  final->drawWrap(p, ss);
}

bool MacroObjectOne::contains(const Coordinate& p, const ScreenInfo& si ) const
{
  return final->contains( p, si );
}

bool MacroObjectOne::inRect(const Rect& r) const
{
  return final->inRect(r);
}

void MacroObjectOne::startMove(const Coordinate& p, const ScreenInfo& si)
{
  final->startMove(p, si);
};

void MacroObjectOne::moveTo(const Coordinate& p)
{
  final->moveTo(p);
}

void MacroObjectOne::stopMove()
{
  final->stopMove();
}

void MacroObjectOne::calcForWidget( const KigWidget& w )
{
  setValidFromChildren();
  cos.calcForWidget( w );
  final->calcForWidget( w );
}

void MacroObjectOne::calc()
{
  cos.calc();
  final->calc();
}

const QCString MacroObjectOne::vBaseTypeName() const
{
  if (!final)
  {
    return hier->getFinElems()[0]->baseTypeName();
  }
  else return final->vBaseTypeName();
};

const QCString MacroObjectOne::vFullTypeName() const
{
  return mtype->fullName();
};

MacroObjectOne::MacroObjectOne(const MacroObjectOne& m)
  : MacroObject( m ), final( 0 ), mtype( m.mtype )
{
  construct();
}

MacroObject::MacroObject( const MacroObject& m )
  : Curve( m ), hier( m.hier ), arguments( m.arguments )
{
  std::for_each( arguments.begin(), arguments.end(),
                 std::bind2nd(
                     std::mem_fun( &Object::addChild ), this ) );
}

Curve* MacroObjectOne::toCurve()
{
  return final->toCurve() ? this : 0;
}

double MacroObjectOne::getParam( const Coordinate& c ) const
{
  assert( toCurve() );
  return final->toCurve()->getParam( c );
}

Coordinate MacroObjectOne::getPoint( double param ) const
{
  assert( toCurve() );
  return final->toCurve()->getPoint( param );
}

const Curve* MacroObjectOne::toCurve() const
{
  return final->toCurve() ? this : 0;
}

void MacroObjectOne::setValidFromChildren()
{
  mvalid = true;
  for ( Objects::const_iterator i = arguments.begin(); i != arguments.end(); ++i )
  {
    mvalid &= (*i)->valid();
  };
}

void MacroObjectOne::construct()
{
  hier->fillUp(arguments);
  final = hier->getFinElems()[0]->actual();
  cos = calcPath( arguments, final );

  cos.calc();
  final->calc();

  for( Objects::iterator i = arguments.begin(); i != arguments.end(); ++i )
    for( Objects::iterator j = cos.begin(); j != cos.end(); ++j )
      ( *i )->delChild( *j );

  for( Objects::iterator i = cos.begin(); i != cos.end(); ++i )
    if ( !arguments.contains( *i ) )
      (*i)->setShown(false);
}
