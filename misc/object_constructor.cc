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
#include "../objects/bogus_imp.h"

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
  bos.calc();
  d.addObjects( bos );
}

void StandardConstructorBase::handlePrelim(
  KigPainter& p, const Objects& os,
  const KigDocument&, const KigWidget&
  ) const
{
  assert ( margsparser.check( os ) != ArgParser::Invalid );
  drawprelim( p, os );
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

void SimpleObjectTypeConstructor::drawprelim( KigPainter& p, const Objects& parents ) const
{
  Args args;
  using namespace std;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), mem_fun( &Object::imp ) );
  ObjectImp* data = mtype->calc( args );
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

MultiObjectTypeConstructor::MultiObjectTypeConstructor(
  const ObjectType* t, const char* descname,
  const char* desc, const char* iconfile,
  const std::vector<int>& params )
  : StandardConstructorBase( descname, desc, iconfile, mparser ),
    mtype( t ), mparams( params ),
    mparser( t->argsParser().without( ObjectImp::ID_IntImp ) )
{
}

MultiObjectTypeConstructor::MultiObjectTypeConstructor(
  const ObjectType* t, const char* descname,
  const char* desc, const char* iconfile,
  int a, int b, int c, int d )
  : StandardConstructorBase( descname, desc, iconfile, mparser ),
    mtype( t ), mparams(),
    mparser( t->argsParser().without( ObjectImp::ID_IntImp ) )
{
  mparams.push_back( a );
  mparams.push_back( b );
  if ( c != -999 ) mparams.push_back( c );
  if ( d != -999 ) mparams.push_back( d );
}

MultiObjectTypeConstructor::~MultiObjectTypeConstructor()
{
}

void MultiObjectTypeConstructor::drawprelim( KigPainter& p,
                                             const Objects& parents ) const
{
  Args args;
  using namespace std;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), mem_fun( &Object::imp ) );

  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( 1 );

  for ( vector<int>::const_iterator i = mparams.begin(); i != mparams.end(); ++i )
  {
    IntImp param( *i );
    args.push_back( &param );
    ObjectImp* data = mtype->calc( args );
    data->draw( p );
    delete data; data = 0;
    args.pop_back();
  };
}

Objects MultiObjectTypeConstructor::build(
  const Objects& os, KigDocument&, KigWidget& ) const
{
  Objects ret;
  using namespace std;
  for ( vector<int>::const_iterator i = mparams.begin(); i != mparams.end(); ++i )
  {
    Args args;
    args.push_back( new IntImp( *i ) );
    Object* n = new Object( mtype, os, args );
    ret.push_back( n );
  };
  return ret;
}

MergeObjectConstructor::~MergeObjectConstructor()
{
  for ( vectype::iterator i = mctors.begin(); i != mctors.end(); ++i )
    delete *i;
}

MergeObjectConstructor::MergeObjectConstructor(
  const char* descname, const char* desc, const char* iconfilename )
  : ObjectConstructor(), mdescname( descname ), mdesc( desc ),
    miconfilename( iconfilename ), mctors()
{
}

ObjectConstructor::~ObjectConstructor()
{
}

void MergeObjectConstructor::merge( ObjectConstructor* e )
{
  mctors.push_back( e );
}

const QString MergeObjectConstructor::descriptiveName() const
{
  return i18n( mdescname );
}

const QString MergeObjectConstructor::description() const
{
  return i18n( mdesc );
}

const QCString MergeObjectConstructor::iconFileName() const
{
  return miconfilename;
}

const int MergeObjectConstructor::wantArgs(
  const Objects& os, const KigDocument& d, const KigWidget& v ) const
{
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int w = (*i)->wantArgs( os, d, v );
    if ( w != ArgsChecker::Invalid ) return w;
  };
  return ArgsChecker::Invalid;
}

void MergeObjectConstructor::handleArgs(
  const Objects& os, KigDocument& d, KigWidget& v ) const
{
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int w = (*i)->wantArgs( os, d, v );
    if ( w == ArgsChecker::Complete )
    {
      (*i)->handleArgs( os, d, v );
      return;
    };
  };
  assert( false );
}

void MergeObjectConstructor::handlePrelim(
  KigPainter& p, const Objects& sel,
  const KigDocument& d, const KigWidget& v ) const
{
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int w = (*i)->wantArgs( sel, d, v );
    if ( w != ArgsChecker::Invalid )
    {
      (*i)->handlePrelim( p, sel, d, v );
      return;
    };
  };
}
