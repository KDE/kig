/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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

QString i18n( const char* );

QString MacroObject::wantArg( const Object* o) const
{
  if( complete ) return 0;
  if( o->vBaseTypeName() != hier->getGegElems()[arguments.size()]->baseTypeName()) return 0;
  else return i18n("Select this %1").arg(o->vTBaseTypeName());
};

bool MacroObject::selectArg(Object* o)
{
  assert(o->vBaseTypeName() == hier->getGegElems()[arguments.size()]->baseTypeName());
  arguments.push_back( o );
  o->addChild(this);
  if (arguments.size() != hier->getGegElems().size()) return false;
  return complete = true;
}

MacroObject::MacroObject(ObjectHierarchy* inHier)
  : hier(inHier)
{
}

MacroObjectOne::MacroObjectOne( ObjectHierarchy* inHier )
  : MacroObject(inHier), final(0), constructed(false)
{
//   assert (inHier->getFinElems().size() == 1);
}

MacroObjectOne::~MacroObjectOne()
{
  delete_all( cos.begin(), cos.end() );
  delete final;
}

void MacroObjectOne::draw(KigPainter& p, bool ss) const
{
  final->setSelected(selected);
  final->setShown(shown);
  final->draw(p, ss);
}

bool MacroObjectOne::contains(const Coordinate& p, const double fault ) const
{
  return final->contains( p, fault );
}

bool MacroObjectOne::inRect(const Rect& r) const
{
  return final->inRect(r);
}

void MacroObjectOne::drawPrelim(KigPainter&, const Object* ) const
{
};

void MacroObjectOne::startMove(const Coordinate& p)
{
  final->startMove(p);
};
void MacroObjectOne::moveTo(const Coordinate& p)
{
  final->moveTo(p);
}
void MacroObjectOne::stopMove()
{
  final->stopMove();
}

void MacroObjectOne::calc( const ScreenInfo& r )
{
  if (!constructed) {
    handleNewObjects( hier->fillUp(arguments), r );
    constructed = true;
  };
  for ( Objects::iterator i = cos.begin(); i != cos.end(); ++i )
    (*i)->calc( r );
  // two times, cause the order in which we calc sometimes sucks
  // TODO: use ObjectHierarchy::calc() here instead.  Problem is that
  // our hier Hierarchy isn't only ours, but shared among all other
  // macro-objects of the same 'type'.  We need to fill it up with our
  // cos first somehow..
  for ( Objects::iterator i = cos.begin(); i != cos.end(); ++i )
    (*i)->calc( r );
  final->calc( r );
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
  if (!final)
  {
    return hier->getFinElems()[0]->fullTypeName();
  }
  else return final->vFullTypeName();
};

void MacroObjectOne::handleNewObjects(const Objects& o, const ScreenInfo& r )
{
  hier->calc( r );
  final = hier->getFinElems()[0]->actual;
  cos = o;
  cos.remove(final);
  for( Objects::iterator i = arguments.begin(); i != arguments.end(); ++i )
    for( Objects::iterator j = cos.begin(); j != cos.end(); ++j )
      ( *i )->delChild( *j );
  for( Objects::iterator i = cos.begin(); i != cos.end(); ++i )
    (*i)->setShown(false);
}

MacroObjectOne::MacroObjectOne(const MacroObjectOne& m)
  : MacroObject( m ), final( 0 ), constructed(false)
{
}

MacroObject::MacroObject( const MacroObject& m )
  : Object( m ), hier( m.hier ), arguments( m.arguments )
{
  std::for_each( arguments.begin(), arguments.end(),
                 std::bind2nd(
                   std::mem_fun( &Object::addChild ), this ) );
}
