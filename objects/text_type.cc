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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "text_type.h"

#include "text_imp.h"
#include "bogus_imp.h"
#include "point_imp.h"
#include "line_imp.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../kig/kig_commands.h"
#include "../modes/label.h"
#include "../misc/coordinate_system.h"

#include <algorithm>

#include <qclipboard.h>
#include <qstringlist.h>

#include <kapplication.h>

#include <cmath>

static const ArgsParser::spec arggspeccs[] =
{
  { IntImp::stype(), "UNUSED", "SHOULD NOT BE SEEN", false },
  { PointImp::stype(), "UNUSED", "SHOULD NOT BE SEEN", false },
  { StringImp::stype(), "UNUSED", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TextType )

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

bool TextType::canMove( const ObjectTypeCalcer& ) const
{
  return true;
}

bool TextType::isFreelyTranslatable( const ObjectTypeCalcer& ) const
{
  return true;
}

void TextType::move( ObjectTypeCalcer& ourobj, const Coordinate& to,
                     const KigDocument& d ) const
{
  const std::vector<ObjectCalcer*> parents = ourobj.parents();
  assert( parents.size() >= 3 );
  const std::vector<ObjectCalcer*> firstthree( parents.begin(), parents.begin() + 3 );
  if( dynamic_cast<ObjectConstCalcer*>( firstthree[1] ) )
  {
    ObjectConstCalcer* c = static_cast<ObjectConstCalcer*>( firstthree[1] );
    c->setImp( new PointImp( to ) );
  }
  else
    firstthree[1]->move( to, d );
}

QStringList TextType::specialActions() const
{
  QStringList ret;
  ret << i18n( "&Copy Text" );
  ret << i18n( "&Toggle Frame" );
  ret << i18n( "&Redefine..." );
  return ret;
}

void TextType::executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                              KigPart& doc, KigWidget&,
                              NormalMode& ) const
{
  std::vector<ObjectCalcer*> parents = c.parents();
  assert( parents.size() >= 3 );

  std::vector<ObjectCalcer*> firstthree( parents.begin(), parents.begin() + 3 );

  assert( mparser.checkArgs( firstthree ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[0] ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[2] ) );

  if ( i == 0 )
  {
    QClipboard* cb = kapp->clipboard();

    // copy the text into the clipboard
    const TextImp* ti = static_cast<const TextImp*>( c.imp() );
    cb->setText( ti->text(), QClipboard::Clipboard );
  }
  else if ( i == 1 )
  {
    // toggle label frame
    int n = (static_cast<const IntImp*>( firstthree[0]->imp() )->data() + 1) % 2;
    KigCommand* kc = new KigCommand( doc, i18n( "Toggle Label Frame" ) );
    kc->addTask( new ChangeObjectConstCalcerTask(
                   static_cast<ObjectConstCalcer*>( firstthree[0] ),
                   new IntImp( n ) ) );
    doc.history()->addCommand( kc );
  }
  else if ( i == 2 )
  {
    assert( dynamic_cast<ObjectTypeCalcer*>( o.calcer() ) );
    // redefine..
    TextLabelRedefineMode m( doc, static_cast<ObjectTypeCalcer*>( o.calcer() ) );
    doc.runMode( &m );
  }
  else assert( false );
}

const ArgsParser& TextType::argParser() const
{
  return mparser;
}

const Coordinate TextType::moveReferencePoint( const ObjectTypeCalcer& ourobj ) const
{
  assert( ourobj.imp()->inherits( TextImp::stype() ) );
  return static_cast<const TextImp*>( ourobj.imp() )->coordinate();
}

std::vector<ObjectCalcer*> TextType::sortArgs( const std::vector<ObjectCalcer*>& os ) const
{
  assert( os.size() >= 3 );
  std::vector<ObjectCalcer*> ret( os.begin(), os.begin() + 3 );
  ret = mparser.parse( ret );
  std::copy( os.begin() + 3,  os.end(), std::back_inserter( ret ) );
  return ret;
}

Args TextType::sortArgs( const Args& args ) const
{
  assert( args.size() >= 3 );
  Args ret( args.begin(), args.begin() + 3 );
  ret = mparser.parse( ret );
  std::copy( args.begin() + 3,  args.end(), std::back_inserter( ret ) );
  return ret;
}

std::vector<ObjectCalcer*> TextType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  const std::vector<ObjectCalcer*> parents = ourobj.parents();
  assert( parents.size() >= 3 );
  std::vector<ObjectCalcer*> ret = parents[1]->movableParents();
  ret.push_back( parents[1] );
  return ret;
}

bool TextType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;
}

