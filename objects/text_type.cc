// text_type.cc
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

#include "text_type.h"

#include "text_imp.h"
#include "bogus_imp.h"
#include "point_imp.h"
#include "object.h"

#include "../kig/kig_view.h"

#include <qstringlist.h>

static const ArgParser::spec arggspeccs[] =
{
  { ObjectImp::ID_IntImp, "UNUSED" },
  { ObjectImp::ID_PointImp, "UNUSED" },
  { ObjectImp::ID_StringImp, "UNUSED" }
};

TextType::TextType()
  : ObjectType( "Label" ), mparser( arggspeccs, 3 )
{
}

TextType::~TextType()
{
}

const TextType* TextType::instance()
{
  static const TextType t;
  return &t;
}

int TextType::resultId() const
{
  return ObjectImp::ID_TextImp;
}

int TextType::impRequirement( const ObjectImp* oi, const Args& args ) const
{
  Args firstthree( args.begin(), args.begin() + 3 );
  if ( mparser.check( firstthree ) == ArgsChecker::Complete )
    return ObjectImp::ID_AnyImp;
  else if ( oi->inherits( ObjectImp::ID_StringImp ) ) return ObjectImp::ID_StringImp;
  else if ( oi->inherits( ObjectImp::ID_PointImp ) ) return ObjectImp::ID_PointImp;
  else return ObjectImp::ID_IntImp;
}

ObjectImp* TextType::calc( const Args& parents, const KigDocument& doc ) const
{
  if( parents.size() < 3 ) return new InvalidImp;
  Args firstthree( parents.begin(), parents.begin() + 3 );
  Args varargs( parents.begin() + 3,  parents.end() );
  Args os = mparser.parse( firstthree );
  if ( ! os[0] || ! os[1] || ! os[2] ) return new InvalidImp;
  assert( os[0]->inherits( ObjectImp::ID_IntImp ) );
  assert( os[1]->inherits( ObjectImp::ID_PointImp ) );
  assert( os[2]->inherits( ObjectImp::ID_StringImp ) );
  int frame = static_cast<const IntImp*>( os[0] )->data();
  bool needframe = frame != 0;
  const Coordinate t = static_cast<const PointImp*>( os[1] )->coordinate();
  QString s = static_cast<const StringImp*>( os[2] )->data();

  for ( Args::iterator i = varargs.begin(); i != varargs.end(); ++i )
  {
    (*i)->fillInNextEscape( s, doc );
  };

  return new TextImp( s, t, needframe );
}

bool TextType::canMove() const
{
  return true;
}

void TextType::move( RealObject* ourobj, const Coordinate&,
                     const Coordinate& dist, const KigDocument& ) const
{
  const Objects parents = ourobj->parents();
  assert( parents.size() >= 3 );
  const Objects firstthree( parents.begin(), parents.begin() + 3 );
  const Objects ps = mparser.parse( firstthree );
  assert( ps[1]->inherits( Object::ID_DataObject ) );
  DataObject* c = static_cast<DataObject*>( ps[1] );
  const PointImp* p = static_cast<const PointImp*>( c->imp() );
  const Coordinate n = p->coordinate() + dist;
  c->setImp( new PointImp( n ) );
}

QStringList TextType::specialActions() const
{
  QStringList ret;
  ret << i18n( "&Toggle Frame" );
  return ret;
}

void TextType::executeAction( int i, RealObject* o, KigDocument& doc, KigWidget& w,
                              NormalMode& ) const
{
  assert( i == 0 );

  Objects parents = o->parents();
  assert( parents.size() >= 3 );

  Objects firstthree( parents.begin(), parents.begin() + 3 );
  Objects os = mparser.parse( firstthree );

  assert( os[0]->hasimp( ObjectImp::ID_IntImp ) );
  assert( os[0]->inherits( Object::ID_DataObject ) );

  int n = (static_cast<const IntImp*>( os[0]->imp() )->data() + 1) % 2;
  static_cast<DataObject*>( os[0] )->setImp( new IntImp( n ) );

  o->calc( doc );
  w.redrawScreen();
}
