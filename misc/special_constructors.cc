// special_constructors.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "special_constructors.h"

#include "i18n.h"
#include "objects.h"
#include "kigpainter.h"

#include "../objects/object.h"
#include "../objects/object_type.h"
#include "../objects/conic_types.h"
#include "../objects/object_imp.h"
#include "../objects/bogus_imp.h"

#include <qpen.h>

#include <algorithm>
#include <functional>

ConicRadicalConstructor::ConicRadicalConstructor()
  : StandardConstructorBase(
    I18N_NOOP( "Radical Line for Conics" ),
    I18N_NOOP( "The lines constructed through the intersections of two conics.  This is also defined for non-intersecting conics." ),
    "conicsradicalline", mparser ),
    mtype( new ConicRadicalType ),
    mparser( mtype->argsParser().without( ObjectImp::ID_IntImp ) )
{
}

ConicRadicalConstructor::~ConicRadicalConstructor()
{
  delete mtype;
}

void ConicRadicalConstructor::drawprelim(
  KigPainter& p, const Objects& parents,
  const KigWidget& w ) const
{
  if ( parents.size() == 2 && parents[0]->has( ObjectImp::ID_ConicImp ) &&
       parents[1]->has( ObjectImp::ID_ConicImp ) )
  {
    Args args;
    p.setBrushStyle( Qt::NoBrush );
    p.setBrushColor( Qt::red );
    p.setPen( QPen ( Qt::red,  1) );
    p.setWidth( 1 );
    using namespace std;
    transform( parents.begin(), parents.end(),
               back_inserter( args ), mem_fun( &Object::imp ) );
    for ( int i = -1; i < 2; i += 2 )
    {
      IntImp root( i );
      IntImp zeroindex( 1 );
      args.push_back( &root );
      args.push_back( &zeroindex );
      ObjectImp* data = mtype->calc( args, w );
      data->draw( p );
      delete data; data = 0;
      args.pop_back();
      args.pop_back();
    };
  };
}

Objects ConicRadicalConstructor::build( const Objects& os, KigDocument&, KigWidget& ) const
{
  Objects ret;
  for ( int i = -1; i < 2; i += 2 )
  {
    Args args;
    args.push_back( new IntImp( i ) );
    args.push_back( new IntImp( 1 ) );
    ret.push_back( new Object( mtype->copy(), os, args ) );
  };
  return ret;
}
