// object_factory.cc
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

#include "object_factory.h"

#include "bogus_imp.h"
#include "curve_imp.h"
#include "point_type.h"
#include "object.h"

#include "../misc/coordinate.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

ObjectFactory* ObjectFactory::s = 0;

RealObject* ObjectFactory::fixedPoint( const Coordinate& c )
{
  DataObject* x = new DataObject( new DoubleImp( c.x ) );
  DataObject* y = new DataObject( new DoubleImp( c.y ) );
  Objects args;
  args.push_back( x );
  args.push_back( y );
  RealObject* o = new RealObject( FixedPointType::instance(), args );
  return o;
}

ObjectFactory* ObjectFactory::instance()
{
  if ( s == 0 ) s = new ObjectFactory();
  return s;
}

RealObject* ObjectFactory::sensiblePoint( const Coordinate& c, const KigDocument& d, const KigWidget& w )
{
  RealObject* o = fixedPoint( c );
  redefinePoint( o, c, d, w );
  return o;
}

void ObjectFactory::redefinePoint( RealObject* point, const Coordinate& c,
                                   const KigDocument& d, const KigWidget& w )
{
  Objects o = d.whatAmIOn( c, w.screenInfo() );
  Object* v = 0;
  // we don't want one of our children as a parent...
  Objects children = point->getAllChildren();
  for ( Objects::iterator i = o.begin(); i != o.end(); ++i )
  {
    if ( (*i)->hasimp( ObjectImp::ID_CurveImp ) && ! children.contains( *i ) )
    {
      v = *i;
      break;
    };
  };
  if ( v )
  {
    // a constrained point...
    DataObject* d = new DataObject( new DoubleImp( static_cast<const CurveImp*>( v->imp() )->getParam( c ) ) );
    point->reset( ConstrainedPointType::instance(), Objects( d ) );
  }
  else
  {
    // a fixed point...
    Objects a;
    a.push_back( new DataObject( new DoubleImp( c.x ) ) );
    a.push_back( new DataObject( new DoubleImp( c.y ) ) );
    point->reset( FixedPointType::instance(), a );
  }
}
