// object_constructor.cc
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

#include "object_constructor.h"

#include "i18n.h"
#include "objects.h"
#include "argsparser.h"
#include "kigpainter.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include "../objects/object_type.h"
#include "../objects/object_imp.h"

#include <qpen.h>

#include <algorithm>
#include <functional>

const QString StandardConstructorBase::descriptiveName() const
{
  return i18n( mdescname );
}

const QString StandardConstructorBase::description() const
{
  return i18n( mdesc );
}

const QCString StandardConstructorBase::iconFileName() const
{
  return miconfile;
}

StandardConstructorBase::StandardConstructorBase(
  const char* descname, const char* desc,
  const char* iconfile, const ArgParser& parser )
  : mdescname( descname ),
    mdesc( desc ),
    miconfile( iconfile ),
    margsparser( parser )
{
}

const int StandardConstructorBase::wantArgs( const Objects& os,
                                             const KigDocument&,
                                             const KigWidget& ) const
{
  return margsparser.check( os );
}

void StandardConstructorBase::handleArgs(
  const Objects& os, KigDocument& d,
  KigWidget& v ) const
{
  Objects args = margsparser.parse( os );
  Objects bos = build( args, d, v );
  bos.calc( v );
  d.addObjects( bos );
}

void StandardConstructorBase::handlePrelim(
  KigPainter& p, const Objects& os,
  const KigDocument&, const KigWidget& v
  ) const
{
  assert ( margsparser.check( os ) != ArgParser::Invalid );

  drawprelim( p, os, v );
}

SimpleObjectTypeConstructor::SimpleObjectTypeConstructor(
  const ObjectType* t, const char* descname,
  const char* desc, const char* iconfile )
  : StandardConstructorBase( descname, desc, iconfile,
                             t->argsParser() ),
    mtype( t )
{
}

SimpleObjectTypeConstructor::~SimpleObjectTypeConstructor()
{
}

void SimpleObjectTypeConstructor::drawprelim( KigPainter& p, const Objects& parents, const KigWidget& w ) const
{
  Args args;
  using namespace std;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), mem_fun( &Object::imp ) );
  ObjectImp* data = mtype->calc( args, w );
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( 1 );
  data->draw( p );
  delete data;
}

Objects SimpleObjectTypeConstructor::build(
  const Objects& os, KigDocument&, KigWidget& ) const
{
  Object* n = new Object( mtype, os, Args() );
  return Objects( n );
}

StandardConstructorBase::~StandardConstructorBase()
{
}

