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

static const ArgParser::spec arggspeccs[] =
{
  { ObjectImp::ID_PointImp, "UNUSED" },
  { ObjectImp::ID_StringImp, "UNUSED" }
};

TextType::TextType()
  : ObjectType( "Label" ), mparser( arggspeccs, 2 )
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
  Args firsttwo( args.begin(), args.begin() + 2 );
  if ( mparser.check( firsttwo ) == ArgsChecker::Complete )
    return ObjectImp::ID_AnyImp;
  else
  {
    return
      oi->inherits( ObjectImp::ID_StringImp ) ? ObjectImp::ID_StringImp : ObjectImp::ID_PointImp;
  };
}

ObjectImp* TextType::calc( const Args& parents, const KigDocument& doc ) const
{
  assert( parents.size() >= 2 );
  Args firsttwo( parents.begin(), parents.begin() + 2 );
  Args varargs( parents.begin() + 2,  parents.end() );
  Args os = mparser.parse( firsttwo );

  assert( os[0]->inherits( ObjectImp::ID_PointImp ) );
  assert( os[1]->inherits( ObjectImp::ID_StringImp ) );
  const Coordinate t = static_cast<const PointImp*>( os[0] )->coordinate();
  QString s = static_cast<const StringImp*>( os[1] )->data();

  for ( Args::iterator i = varargs.begin(); i != varargs.end(); ++i )
  {
    (*i)->fillInNextEscape( s, doc );
  };

  return new TextImp( s, t );
}

bool TextType::canMove() const
{
  return true;
}

void TextType::move( RealObject* ourobj, const Coordinate&,
                     const Coordinate& dist ) const
{
  const Objects parents = ourobj->parents();
  assert( parents.size() >= 2 );
  const Objects firsttwo( parents.begin(), parents.begin() + 2 );
  const Objects ps = mparser.parse( firsttwo );
  assert( ps.front()->inherits( Object::ID_DataObject ) );
  DataObject* c = static_cast<DataObject*>( ps.front() );
  const PointImp* p = static_cast<const PointImp*>( c->imp() );
  const Coordinate n = p->coordinate() + dist;
  c->setImp( new PointImp( n ) );
}
