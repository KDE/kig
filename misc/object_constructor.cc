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
#include "guiaction.h"

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
  bos.calc( d );
  d.addObjects( bos );
}

void StandardConstructorBase::handlePrelim(
  KigPainter& p, const Objects& os,
  const KigDocument& d, const KigWidget&
  ) const
{
  assert ( margsparser.check( os ) != ArgParser::Invalid );
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( -1 ); // -1 means the default width for the object being
                    // drawn..
  drawprelim( p, os, d );
}

SimpleObjectTypeConstructor::SimpleObjectTypeConstructor(
  const ArgparserObjectType* t, const char* descname,
  const char* desc, const char* iconfile )
  : StandardConstructorBase( descname, desc, iconfile,
                             t->argParser() ),
    mtype( t )
{
}

SimpleObjectTypeConstructor::~SimpleObjectTypeConstructor()
{
}

void SimpleObjectTypeConstructor::drawprelim( KigPainter& p, const Objects& parents,
                                              const KigDocument& doc ) const
{
  Args args;
  using namespace std;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), mem_fun( &Object::imp ) );
  ObjectImp* data = mtype->calc( args, doc );
  data->draw( p );
  delete data;
}

Objects SimpleObjectTypeConstructor::build(
  const Objects& os, KigDocument&, KigWidget& ) const
{
  RealObject* n = new RealObject( mtype, os );
  return Objects( n );
}

StandardConstructorBase::~StandardConstructorBase()
{
}

MultiObjectTypeConstructor::MultiObjectTypeConstructor(
  const ArgparserObjectType* t, const char* descname,
  const char* desc, const char* iconfile,
  const std::vector<int>& params )
  : StandardConstructorBase( descname, desc, iconfile, mparser ),
    mtype( t ), mparams( params ),
    mparser( t->argParser().without( ObjectImp::ID_IntImp ) )
{
}

MultiObjectTypeConstructor::MultiObjectTypeConstructor(
  const ArgparserObjectType* t, const char* descname,
  const char* desc, const char* iconfile,
  int a, int b, int c, int d )
  : StandardConstructorBase( descname, desc, iconfile, mparser ),
    mtype( t ), mparams(),
    mparser( t->argParser().without( ObjectImp::ID_IntImp ) )
{
  mparams.push_back( a );
  mparams.push_back( b );
  if ( c != -999 ) mparams.push_back( c );
  if ( d != -999 ) mparams.push_back( d );
}

MultiObjectTypeConstructor::~MultiObjectTypeConstructor()
{
}

void MultiObjectTypeConstructor::drawprelim( KigPainter& p, const Objects& parents,
                                             const KigDocument& doc ) const
{
  Args args;
  using namespace std;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), mem_fun( &Object::imp ) );

  for ( vector<int>::const_iterator i = mparams.begin(); i != mparams.end(); ++i )
  {
    IntImp param( *i );
    args.push_back( &param );
    ObjectImp* data = mtype->calc( args, doc );
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
    Objects args;
    args.push_back( new DataObject( new IntImp( *i ) ) );
    ret.push_back( args.back() );
    copy( os.begin(), os.end(), back_inserter( args ) );
    RealObject* n = new RealObject( mtype, args );
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

QString StandardConstructorBase::useText( const Object& o, const Objects& sel,
                                          const KigDocument&, const KigWidget& ) const
{
  using namespace std;
  Args args;
  transform( sel.begin(), sel.end(), back_inserter( args ), mem_fun( &Object::imp ) );

  const char* ret = margsparser.usetext( o.imp(), args );
  if ( ! ret ) return QString::null;
  return i18n( ret );
}

QString MergeObjectConstructor::useText( const Object& o, const Objects& sel,
                                         const KigDocument& d, const KigWidget& v ) const
{
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int w = (*i)->wantArgs( sel.with( const_cast<Object*>( &o ) ), d, v );
    if ( w != ArgsChecker::Invalid ) return (*i)->useText( o, sel, d, v );
  };
  return QString::null;
}

MacroConstructor::MacroConstructor( const ObjectHierarchy& hier, const QString& name,
                                    const QString& desc, const QCString& iconfile )
  : ObjectConstructor(), mhier( hier ), mname( name ), mdesc( desc ),
    mbuiltin( false ), miconfile( iconfile ),
    mparser( mhier.argParser() )
{
}

MacroConstructor::MacroConstructor(
  const Objects& input, const Objects& output,
  const QString& name, const QString& description,
  const QCString& iconfile )
  : ObjectConstructor(), mhier( input, output ),
    mname( name ), mdesc( description ), mbuiltin( false ),
    miconfile( iconfile ),
    mparser( mhier.argParser() )
{
}

MacroConstructor::~MacroConstructor()
{
}

const QString MacroConstructor::descriptiveName() const
{
  return mname;
}

const QString MacroConstructor::description() const
{
  return mdesc;
}

const QCString MacroConstructor::iconFileName() const
{
  return miconfile.isNull() ? QCString( "gear" ) : miconfile;
}

const int MacroConstructor::wantArgs( const Objects& os, const KigDocument&,
                                      const KigWidget& ) const
{
  return mparser.check( os );
}

void MacroConstructor::handleArgs( const Objects& os, KigDocument& d,
                                   KigWidget& ) const
{
  Objects args = mparser.parse( os );
  Objects bos = mhier.buildObjects( args, d );
  bos.calc( d );
  d.addObjects( bos );
}

QString MacroConstructor::useText( const Object& o, const Objects& sel,
                                   const KigDocument&, const KigWidget&
  ) const
{
  using namespace std;
  Args args;
  transform( sel.begin(), sel.end(), back_inserter( args ),
             mem_fun( &Object::imp ) );
  return mparser.usetext( o.imp(), args );
}

void MacroConstructor::handlePrelim( KigPainter& p, const Objects& sel,
                                     const KigDocument& doc, const KigWidget&
  ) const
{
  if ( sel.size() != mhier.numberOfArgs() ) return;

  using namespace std;
  Args args;
  transform( sel.begin(), sel.end(), back_inserter( args ),
             mem_fun( &Object::imp ) );
  args = mparser.parse( args );
  std::vector<ObjectImp*> ret = mhier.calc( args, doc );
  for ( uint i = 0; i < ret.size(); ++i )
  {
    p.setBrushStyle( Qt::NoBrush );
    p.setBrushColor( Qt::red );
    p.setPen( QPen ( Qt::red,  1) );
    p.setWidth( -1 ); // -1 means the default width for the object being
                      // drawn..
    ret[i]->draw( p );
    delete ret[i];
  };
}

void SimpleObjectTypeConstructor::plug( KigDocument*, KigGUIAction* )
{
}

void MultiObjectTypeConstructor::plug( KigDocument*, KigGUIAction* )
{
}

void MergeObjectConstructor::plug( KigDocument*, KigGUIAction* )
{
}

void MacroConstructor::plug( KigDocument* doc, KigGUIAction* kact )
{
  if ( mbuiltin ) return;
  if ( mhier.numberOfResults() != 1 )
    doc->aMNewOther.append( kact );
  else
  {
    switch( mhier.idOfLastResult() )
    {
    case ObjectImp::ID_SegmentImp: doc->aMNewSegment.append( kact ); break;
    case ObjectImp::ID_PointImp: doc->aMNewPoint.append( kact ); break;
    case ObjectImp::ID_CircleImp: doc->aMNewCircle.append( kact ); break;
    case ObjectImp::ID_LineImp: doc->aMNewLine.append( kact ); break;
    case ObjectImp::ID_ConicImp: doc->aMNewConic.append( kact ); break;
    default: doc->aMNewOther.append( kact ); break;
    };
  };
  doc->aMNewAll.append( kact );
}

const ObjectHierarchy& MacroConstructor::hierarchy() const
{
  return mhier;
}

bool SimpleObjectTypeConstructor::isTransform() const
{
  return mtype->isTransform();
}

bool MultiObjectTypeConstructor::isTransform() const
{
  return mtype->isTransform();
}

bool MergeObjectConstructor::isTransform() const
{
  bool ret = false;
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
    ret |= (*i)->isTransform();
  return ret;
}

bool MacroConstructor::isTransform() const
{
  return false;
}

void MacroConstructor::setBuiltin( bool builtin )
{
  mbuiltin = builtin;
}

