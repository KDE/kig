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

#include "point.h"
#include "../misc/hierarchy.h"

QString MacroObject::wantArg( const Object* o) const
{
  if( complete ) return 0;
  if( o->vBaseTypeName() != hier->getGegElems()[arguments.size()]->getTypeName()) return 0;
  else return i18n("Select this %1").arg(o->vTBaseTypeName());
};

QString MacroObject::wantPoint() const
{
  if ( hier->getGegElems()[arguments.size()]->getTypeName() == Point::sBaseTypeName() ) return i18n( "Select this point" );
  return 0;
}

bool MacroObject::selectArg(Object* o)
{
  assert(o->vBaseTypeName() == hier->getGegElems()[arguments.size()]->getTypeName());
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

void MacroObjectOne::calc()
{
  if (!constructed) {
    handleNewObjects( hier->fillUp(arguments) );
    constructed = true;
  };
  std::for_each( cos.begin(), cos.end(), std::mem_fun( &Object::calc ) );
  // two times, cause the order in which we calc sometimes sucks
  std::for_each( cos.begin(), cos.end(), std::mem_fun( &Object::calc ) );
  final->calc();
}

const QCString MacroObjectOne::vBaseTypeName() const
{
  if (!final)
    {
      // actually this returns a FullTypeName(), but i'm hoping this
      // part of the function won't get used too often
      return hier->getFinElems()[0]->getTypeName();
    }
  else return final->vBaseTypeName();
};

const QCString MacroObjectOne::vFullTypeName() const
{
  if (!final)
    {
      return hier->getFinElems()[0]->getTypeName();
    }
  else return final->vFullTypeName();
};

void MacroObjectOne::handleNewObjects(const Objects& o)
{
  hier->calc();
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
  : MacroObject(m.hier), final(0), constructed(false)
{
  arguments=m.arguments;
  for( Objects::iterator i = arguments.begin(); i != arguments.end(); ++i )
    (*i)->addChild(this);
  complete = m.complete;
  if (complete) calc();
}
