// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "text_type.h"

#include "text_imp.h"
#include "bogus_imp.h"
#include "object_drawer.h"
#include "point_imp.h"
#include "line_imp.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../kig/kig_commands.h"
#include "../modes/label.h"
#include "../misc/coordinate_system.h"

#include <algorithm>
#include <cmath>
#include <iterator>

#include <QApplication>
#include <QClipboard>
#include <QStringList>
#include <QFontDialog>

static const ArgsParser::spec arggspeccs[] =
{
  { IntImp::stype(), "UNUSED", "SHOULD NOT BE SEEN", false },
  { PointImp::stype(), "UNUSED", "SHOULD NOT BE SEEN", false },
  { StringImp::stype(), "UNUSED", "SHOULD NOT BE SEEN", false }
};

// KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( GenericTextType )

GenericTextType::GenericTextType( const char fulltypename[] )
  : ObjectType( fulltypename ), mparser( arggspeccs, 3 )
{
}

GenericTextType::~GenericTextType()
{
}

const ObjectImpType* GenericTextType::resultId() const
{
  return TextImp::stype();
}

const ObjectImpType* GenericTextType::impRequirement( const ObjectImp* o, const Args& args ) const
{
  assert( args.size() >= 3 );
  Args firstthree( args.begin(), args.begin() + 3 );
  if ( o == args[0] || o == args[1] || o == args[2] )
    return argParser().impRequirement( o, firstthree );
  else
    return ObjectImp::stype();
}

ObjectImp* GenericTextType::calc( const Args& parents, const KigDocument& doc ) const
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

  if ( varargs.size() == 1 && varargs[0]->inherits( DoubleImp::stype() ) )
  {
    double value = static_cast<const DoubleImp*>( varargs[0] )->data();
    return new NumericTextImp( s, t, needframe, value );
  } else if ( varargs.size() == 1 && varargs[0]->inherits( TestResultImp::stype() ) )
  {
    bool value = static_cast<const TestResultImp*>( varargs[0] )->truth();
    return new BoolTextImp( s, t, needframe, value );
  } else {
    return new TextImp( s, t, needframe );
  }
}

bool GenericTextType::canMove( const ObjectTypeCalcer& ) const
{
  return true;
}

bool GenericTextType::isFreelyTranslatable( const ObjectTypeCalcer& ) const
{
  return true;
}

void GenericTextType::move( ObjectTypeCalcer& ourobj, const Coordinate& to,
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

const ArgsParser& GenericTextType::argParser() const
{
  return mparser;
}

const Coordinate GenericTextType::moveReferencePoint( const ObjectTypeCalcer& ourobj ) const
{
  assert( ourobj.imp()->inherits( TextImp::stype() ) );
  return static_cast<const TextImp*>( ourobj.imp() )->coordinate();
}

std::vector<ObjectCalcer*> GenericTextType::sortArgs( const std::vector<ObjectCalcer*>& os ) const
{
  assert( os.size() >= 3 );
  std::vector<ObjectCalcer*> ret( os.begin(), os.begin() + 3 );
  ret = mparser.parse( ret );
  std::copy( os.begin() + 3,  os.end(), std::back_inserter( ret ) );
  return ret;
}

Args GenericTextType::sortArgs( const Args& args ) const
{
  assert( args.size() >= 3 );
  Args ret( args.begin(), args.begin() + 3 );
  ret = mparser.parse( ret );
  std::copy( args.begin() + 3,  args.end(), std::back_inserter( ret ) );
  return ret;
}

std::vector<ObjectCalcer*> GenericTextType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  const std::vector<ObjectCalcer*> parents = ourobj.parents();
  assert( parents.size() >= 3 );
  std::vector<ObjectCalcer*> ret = parents[1]->movableParents();
  ret.push_back( parents[1] );
  return ret;
}

bool GenericTextType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;
}

QStringList GenericTextType::specialActions() const
{
  QStringList ret;
  ret << i18n( "&Copy Text" );
  ret << i18n( "&Toggle Frame" );
  ret << i18n( "Set &Font..." );
  return ret;
}

void GenericTextType::executeAction( int i, ObjectHolder& oh, ObjectTypeCalcer& c,
                              KigPart& doc, KigWidget& w,
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
    QClipboard* cb = QApplication::clipboard();

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
    doc.history()->push( kc );
  }
  else if ( i == 2 )
  {
    // change label font
    QFont f = oh.drawer()->font();
    bool result;
    f = QFontDialog::getFont( &result, f, &w );
    if ( !result ) return;
    KigCommand* kc = new KigCommand( doc, i18n( "Change Label Font" ) );
    kc->addTask( new ChangeObjectDrawerTask( &oh, oh.drawer()->getCopyFont( f ) ) );
    doc.history()->push( kc );
  }
  else assert( false );
}

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TextType )

TextType::TextType()
  : GenericTextType( "Label" )
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

QStringList TextType::specialActions() const
{
  QStringList ret = GenericTextType::specialActions();
  ret << i18n( "&Redefine..." );
  return ret;
}

void TextType::executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                              KigPart& doc, KigWidget& w,
                              NormalMode& nm ) const
{
  std::vector<ObjectCalcer*> parents = c.parents();
  assert( parents.size() >= 3 );

  std::vector<ObjectCalcer*> firstthree( parents.begin(), parents.begin() + 3 );

  assert( argParser().checkArgs( firstthree ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[0] ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[2] ) );

  const int parentactions = GenericTextType::specialActions().count();
  if ( i < parentactions )
    GenericTextType::executeAction( i, o, c, doc, w, nm );
  else if ( i == parentactions )
  {
    assert( dynamic_cast<ObjectTypeCalcer*>( o.calcer() ) );
    // redefine..
    TextLabelRedefineMode m( doc, static_cast<ObjectTypeCalcer*>( o.calcer() ) );
    doc.runMode( &m );
  }
  else assert( false );
}


KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( NumericTextType )

NumericTextType::NumericTextType()
  : GenericTextType( "NumericLabel" )
{
}

NumericTextType::~NumericTextType()
{
}

const NumericTextType* NumericTextType::instance()
{
  static const NumericTextType t;
  return &t;
}

QStringList NumericTextType::specialActions() const
{
  QStringList ret = GenericTextType::specialActions();
  ret << i18n( "Change &Value..." );
  return ret;
}

void NumericTextType::executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                              KigPart& doc, KigWidget& w,
                              NormalMode& nm) const
{
  std::vector<ObjectCalcer*> parents = c.parents();
  assert( parents.size() == 4 );

  std::vector<ObjectCalcer*> firstthree( parents.begin(), parents.begin() + 3 );

  assert( o.imp()->inherits( NumericTextImp::stype() ) );
  assert( argParser().checkArgs( firstthree ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[0] ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[2] ) );

  const int parentactions = GenericTextType::specialActions().count();
  if ( i < parentactions )
    GenericTextType::executeAction( i, o, c, doc, w, nm );
  else if ( i == parentactions )
  {
    bool ok;
    ObjectConstCalcer* valuecalcer = dynamic_cast<ObjectConstCalcer*>( parents[3] );
    assert( valuecalcer );
    double oldvalue = static_cast<const NumericTextImp*>( o.imp() )->getValue();
    double value = getDoubleFromUser(
      i18n( "Set Value" ), i18n( "Enter the new value:" ),
      oldvalue, &w, &ok, -2147483647, 2147483647, 7 );
    if ( ! ok ) return;
    MonitorDataObjects mon( parents );
    valuecalcer->setImp( new DoubleImp( value ) );
    KigCommand* kc = new KigCommand( doc, i18n( "Change Displayed Value" ) );
    mon.finish( kc );
    doc.history()->push( kc );
  }
  else assert( false );
}

