// coordproppoint.cc
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

#include "coordproppoint.h"
#include "../misc/common.h"
#include "../misc/i18n.h"

CoordinatePropertyPoint::CoordinatePropertyPoint( const Objects& os )
  : mparent( 0 ), mpropindex( 10000 )
{
  assert( os.size() == 1 );
  mparent = os[0];
  mparent->addChild( this );
}

CoordinatePropertyPoint::CoordinatePropertyPoint( const CoordinatePropertyPoint& p )
  : Point( p ), mparent( p.mparent ), mpropindex( p.mpropindex )
{
  mparent->addChild( this );
}

CoordinatePropertyPoint::~CoordinatePropertyPoint()
{
}

Object::prop_map CoordinatePropertyPoint::getParams()
{
  prop_map m = Point::getParams();
  m["which-property"] = QString::fromLatin1( mparent->properties()[mpropindex] );
  return m;
}

void CoordinatePropertyPoint::setParams( const prop_map& m )
{
  Point::setParams( m );
  prop_map::const_iterator p = m.find("which-property");
  assert( p != m.end() );
  int index = mparent->properties().findIndex( p->second.latin1() );
  assert( index != -1 );
  mpropindex = static_cast<uint>( index );
}

const QCString CoordinatePropertyPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QString CoordinatePropertyPoint::vDescriptiveName() const
{
  return i18n( mparent->properties()[mpropindex] );
}

const QString CoordinatePropertyPoint::vDescription() const
{
  return vDescriptiveName();
}

const QCString CoordinatePropertyPoint::vIconFileName() const
{
  return "point4";
}

const int CoordinatePropertyPoint::vShortCut() const
{
  return 0;
}

Objects CoordinatePropertyPoint::getParents() const
{
  return Objects( mparent );
}

void CoordinatePropertyPoint::calcForWidget( const KigWidget& w )
{
  assert( mpropindex != 10000 );
  Property p = mparent->property( mpropindex, w );
  assert( p.type() == Property::Coord );
  mC = p.coordData();
}

CoordinatePropertyPoint::CoordinatePropertyPoint( Object* o, uint propindex )
  : Point(), mparent( o ), mpropindex( propindex )
{
  mparent->addChild( this );
}

CoordinatePropertyPoint::CoordinatePropertyPoint( Object* o, const QCString& propstring )
  : Point(), mparent( o ), mpropindex( 10000 )
{
  int index = o->properties().findIndex( propstring );
  assert( index != -1 );
  mpropindex = static_cast<uint>( index );
}

void CoordinatePropertyPoint::calc()
{
}

