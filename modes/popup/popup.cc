/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "popup.h"

#include "../../kig/kig_part.h"
#include "../../kig/kig_document.h"
#include "../../kig/kig_view.h"
#include "../../kig/kig_commands.h"
#include "../../objects/object_imp.h"
#include "../../objects/object_drawer.h"
#include "../../objects/bogus_imp.h"
#include "../../objects/point_imp.h"
#include "../../objects/line_imp.h"
#include "../../objects/other_type.h"
#include "../../objects/object_factory.h"
#include "../../objects/polygon_imp.h"
#include "../../objects/text_imp.h"
#include "../../objects/text_type.h"
#include "../../misc/lists.h"
#include "../../misc/argsparser.h"
#include "../../misc/kigpainter.h"
#include "../../misc/coordinate_system.h"
#include "../../misc/object_constructor.h"
#include "../construct_mode.h"
#include "../normal.h"
#include "../moving.h"

#include "builtindocumentactionsprovider.h"
#include "builtinobjectactionsprovider.h"
#include "nameobjectactionsprovider.h"
#include "objectconstructoractionsprovider.h"
#include "objecttypeactionsprovider.h"
#include "propertiesactionsprovider.h"

#include <KIconEngine>

#include <config-kig.h>

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "scriptactionsprovider.h"
#endif

#define MAXMENUITEMS 20
/*
 * mp: if a submenus requires more than MAXMENUITEMS items then a new
 * submenu titled " more..." is added
 */

using namespace std;

NormalModePopupObjects::NormalModePopupObjects( KigPart& part,
                                                KigWidget& view,
                                                NormalMode& mode,
                                                const std::vector<ObjectHolder*>& objs,
                                                const QPoint& plc )
  : QMenu( &view ), mplc( plc ), mpart( part ), mview( view ), mobjs( objs ),
    mmode( mode ), monlylabels( false )
{
  bool empty = objs.empty();
  bool single = objs.size() == 1;

  QString title;
  if ( empty )
    title = i18n( "Kig Document" );
  else if ( single )
  {
    if ( !objs[0]->name().isNull() )
      title = QStringLiteral( "%1 %2" ).arg( objs[0]->imp()->type()->translatedName() ).arg( objs[0]->name() );
    else
      title = objs[0]->imp()->type()->translatedName();
  }
  else
    title = i18np( "%1 Object", "%1 Objects", objs.size() );
  addSection( title );

  if ( !empty )
  {
    monlylabels = true;
    uint i = 0;
    while ( i < objs.size() && monlylabels )
    {
      monlylabels &= objs[i]->imp()->inherits( TextImp::stype() );
      ++i;
    }
  }

  if ( empty )
  {
    // provides some diverse stuff like "unhide all", set coordinate
    // system etc.
    mproviders.push_back( new BuiltinDocumentActionsProvider() );
  };
  // construct an object using these objects and start constructing an
  // object using these objects
  mproviders.push_back( new ObjectConstructorActionsProvider() );
  if ( single )
    mproviders.push_back( new NameObjectActionsProvider() );
  if ( ! empty )
  {
    // stuff like hide, show, delete, set size, set color..
    mproviders.push_back( new BuiltinObjectActionsProvider() );
    // show property as text label -> show menu
    // and construct property's as objects -> construct menu
    mproviders.push_back( new PropertiesActionsProvider() );
    // stuff like "redefine point" for a fixed or constrained point..
    mproviders.push_back( new ObjectTypeActionsProvider() );
  }
#ifdef KIG_ENABLE_PYTHON_SCRIPTING
  // script action..
  mproviders.push_back( new ScriptActionsProvider() );
#endif

  static const QString menunames[NumberOfMenus] =
    {
      i18n( "&Transform" ),
      i18n( "T&est" ),
      i18n( "Const&ruct" ),
      i18n( "&Start" ),
      i18n( "Add Te&xt Label" ),
      i18n( "Set Co&lor" ),
      i18n( "Set &Pen Width" ),
      i18n( "Set St&yle" ),
      QString(),
      i18n( "Set Coordinate S&ystem" )
    };
  static const QString menuicons[NumberOfMenus] =
    {
      "centralsymmetry",
      "test",
      QString(),
      "system-run",
      "kig_text", // <- draw-text... hope I manage to rename it in time
      "format-fill-color",
//      "colorize",
      "sizer",
      "draw-brush",
      QString(),
      QString()
    };

  // creating the menus and setting their title and icon
  KIconLoader* l = part.iconLoader();
  for ( uint i = 0; i < NumberOfMenus; ++i )
  {
    if ( i == ToplevelMenu ) continue;
    mmenus[i] = mmenuslast[i] = new QMenu( this );
    if ( !menunames[i].isEmpty() )
      mmenus[i]->setTitle( menunames[i] );
    if ( !menuicons[i].isEmpty() )
    {
      mmenus[i]->setIcon( QIcon( new KIconEngine( menuicons[i], l ) ) );
    }
  }
  mmenus[ToplevelMenu] = mmenuslast[ToplevelMenu] = this;
  /*
   * mp: previously ToplevelMenu was treated like a submenu (of itself).
   * unfortunately this had as side effect a duplicated triggering of its
   * actions with unpredictable consequences.  In this way the addAction
   * in addInternalAction directly adds the action at toplevel.
   */

  connect( this, &QMenu::triggered,
           this, &NormalModePopupObjects::toplevelMenuSlot );

  for ( int i = 0; i < NumberOfMenus; ++i )
  {
    int nextfree = 10;
    for ( uint j = 0; j < mproviders.size(); ++j )
      mproviders[j]->fillUpMenu( *this, i, nextfree );
  };
  QAction* firstrealaction = actions()[1];
  for ( int i = 0; i < NumberOfMenus; ++i )
  {
    if ( i == ToplevelMenu ) continue;
    if ( mmenus[i]->actions().count() == 0 ) continue;
    //addMenu( mmenus[i] );
    insertMenu( firstrealaction, mmenus[i] );
  };
  // addActions( mmenus[ToplevelMenu]->actions() );
}

void NormalModePopupObjects::toplevelMenuSlot( QAction* act )
{
//  activateAction( ToplevelMenu, i );
  int data = act->data().toInt();
  int id = data & 0xFF;
  int menu = data >> 8;
qDebug() << "menu: " << menu << " - id: " << id;
  activateAction( menu, id );
}

void NormalModePopupObjects::activateAction( int menu, int action )
{
  bool done = false;
  // we need action - 10 cause we called fillUpMenu with nextfree set
  // to 10 initially..
  action -= 10;
qDebug() << "MENU: " << menu << " - ACTION: " << action;
  for ( uint i = 0; ! done && i < mproviders.size(); ++i )
    done = mproviders[i]->executeAction( menu, action, mobjs, *this, mpart, mview, mmode );
}

NormalModePopupObjects::~NormalModePopupObjects()
{
  delete_all ( mproviders.begin(), mproviders.end() );
}

QAction* NormalModePopupObjects::addInternalAction( int menu, const QIcon& pix, int id )
{
  return addInternalAction( menu, pix, QLatin1String(""), id );
}

QAction* NormalModePopupObjects::addInternalAction( int menu, const QIcon& icon, const QString& name, int id )
{
  if ( mmenuslast[menu]->actions().size() >= MAXMENUITEMS )
    mmenuslast[menu] = mmenuslast[menu]->addMenu( i18nc( "More menu items", "More..." ) );
  QAction* newaction = mmenuslast[menu]->addAction( icon, name );
  newaction->setData( QVariant::fromValue( ( menu << 8 ) | id ) );
  return newaction;
}

QAction* NormalModePopupObjects::addInternalAction( int menu, const QString& name, int id )
{
  if ( mmenuslast[menu]->actions().size() >= MAXMENUITEMS )
    mmenuslast[menu] = mmenuslast[menu]->addMenu( i18nc( "More menu items", "More..." ) );
  QAction* newaction = mmenuslast[menu]->addAction( name );
  newaction->setData( QVariant::fromValue( ( menu << 8 ) | id ) );
  return newaction;
}

QAction* NormalModePopupObjects::addInternalAction( int menu, QAction* act )
{
  if ( mmenuslast[menu]->actions().size() >= MAXMENUITEMS )
    mmenuslast[menu] = mmenuslast[menu]->addMenu( i18nc( "More menu items", "More..." ) );
  mmenuslast[menu]->addAction( act );
  return act;
}
