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
#include "../kig/kig_part.h"
#include "../kig/kig_commands.h"
#include "../modes/label.h"
#include "../misc/coordinate_system.h"

#include <algorithm>

#include <qstringlist.h>

static const ArgsParser::spec arggspeccs[] =
{
  { IntImp::stype(), "UNUSED" },
  { PointImp::stype(), "UNUSED" },
  { StringImp::stype(), "UNUSED" }
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

const ObjectImpType* TextType::resultId() const
{
  return TextImp::stype();
}

const ObjectImpType* TextType::impRequirement( const ObjectImp* o, const Args& args ) const
{
  assert( args.size() >= 3 );
  Args firstthree( args.begin(), args.begin() + 3 );
  if ( o == args[0] || o == args[1] || o == args[2] )
    return mparser.impRequirement( o, firstthree );
  else
    return ObjectImp::stype();
}

ObjectImp* TextType::calc( const Args& parents, const KigDocument& doc ) const
{
  if( parents.size() < 3 ) return new InvalidImp;
  Args firstthree( parents.begin(), parents.begin() + 3 );
  Args varargs( parents.begin() + 3,  parents.end() );

  if ( ! mparser.checkArgs( firstthree ) ) return new InvalidImp;

  int frame = static_cast<const IntImp*>( firstthree[0] )->data();
  bool needframe = frame != 0;
  const Coordinate t = static_cast<const PointImp*>( firstthree[1] )->coordinate();
  QString s = static_cast<const StringImp*>( firstthree[2] )->data();

  for ( Args::iterator i = varargs.begin(); i != varargs.end(); ++i )
    (*i)->fillInNextEscape( s, doc );

  return new TextImp( s, t, needframe );
}

bool TextType::canMove() const
{
  return true;
}

void TextType::move( RealObject* ourobj, const Coordinate& to,
                     const KigDocument& d ) const
{
  const Objects parents = ourobj->parents();
  assert( parents.size() >= 3 );
  const Objects firstthree( parents.begin(), parents.begin() + 3 );
  if( firstthree[1]->inherits( Object::ID_DataObject ) )
  {
    DataObject* c = static_cast<DataObject*>( firstthree[1] );
    c->setImp( new PointImp( to ) );
  }
  else
    firstthree[1]->move( to, d );
}

QStringList TextType::specialActions() const
{
  QStringList ret;
  ret << i18n( "&Toggle Frame" );
  ret << i18n( "&Redefine..." );
  return ret;
}

void TextType::executeAction( int i, RealObject* o, KigDocument& doc, KigWidget& w,
                              NormalMode& ) const
{
  Objects parents = o->parents();
  assert( parents.size() >= 3 );

  Objects firstthree( parents.begin(), parents.begin() + 3 );

  assert( mparser.checkArgs( firstthree ) );
  assert( firstthree[0]->inherits( Object::ID_DataObject ) );
  assert( firstthree[2]->inherits( Object::ID_DataObject ) );

  if ( i == 0 )
  {
    // toggle label frame
    Objects monos( firstthree[0] );
    MonitorDataObjects mon( monos );
    int n = (static_cast<const IntImp*>( firstthree[0]->imp() )->data() + 1) % 2;
    static_cast<DataObject*>( firstthree[0] )->setImp( new IntImp( n ) );
    KigCommand* kc = new KigCommand( doc, i18n( "Toggle Label Frame" ) );
    kc->addTask( mon.finish() );
    doc.history()->addCommand( kc );
  }
  else if ( i == 1 )
  {
    // redefine..
    TextLabelRedefineMode m( doc, o );
    doc.runMode( &m );
    w.redrawScreen();
  }
  else assert( false );
}

const ArgsParser& TextType::argParser() const
{
  return mparser;
}

const Coordinate TextType::moveReferencePoint( const RealObject* ourobj ) const
{
  if ( ourobj->hasimp( TextImp::stype() ) )
    return static_cast<const TextImp*>( ourobj->imp() )->coordinate();
  else return Coordinate::invalidCoord();
}

Objects TextType::sortArgs( const Objects& os ) const
{
  assert( os.size() >= 3 );
  Objects ret( os.begin(), os.begin() + 3 );
  ret = mparser.parse( ret );
  std::copy( os.begin() + 3,  os.end(), std::back_inserter( ret ) );
  return ret;
}
