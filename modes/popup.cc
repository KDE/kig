/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "popup.h"
#include "popup.moc"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"
#include "../misc/i18n.h"
#include "../objects/object_imp.h"
#include "../objects/object.h"
#include "../objects/other_type.h"
#include "../objects/object_factory.h"
#include "../misc/lists.h"
#include "../misc/argsparser.h"
#include "../misc/coordinate_system.h"
#include "../misc/object_constructor.h"
#include "construct_mode.h"
#include "normal.h"
#include "moving.h"

#include <qcursor.h>
#include <qpainter.h>
#include <qpen.h>
#include <qdialog.h>
#include <kglobal.h>
#include <kiconloader.h>

#include <algorithm>
#include <functional>

using namespace std;

class NormalModePopupObjects;

/**
 * This class is an abstract class.  Its role is to fill up the
 * NormalModePopupObjects with useful actions..
 */
class PopupActionProvider
{
public:
  virtual ~PopupActionProvider();
  /**
   * add all your entries to menu menu in popup popup.  Set nextfree
   * to the next free index..
   */
  virtual void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree ) = 0;
  /**
   * try to execute the id'th action you added to menu menu in popup
   * popup ( first is 0 ).  Return true if this action does indeed
   * belong to you ( is not greater than the number of actions you
   * added ).  Else return false, and subtract the number of actions
   * you added from id.  This requires you to keep a record of how
   * much actions you added ( unless it's a fixed number, of course
   * ).
   */
  virtual bool executeAction( int menu, int& id, const Objects& os,
                              NormalModePopupObjects& popup,
                              KigDocument& doc, KigWidget& w, NormalMode& m ) = 0;
};

class BuiltinObjectActionsProvider
  : public PopupActionProvider
{
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const Objects& os,
                      NormalModePopupObjects& popup,
                      KigDocument& doc, KigWidget& w, NormalMode& m );
};

class BuiltinDocumentActionsProvider
  : public PopupActionProvider
{
  int mnumberofcoordsystems;
  bool misfullscreen;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const Objects& os,
                      NormalModePopupObjects& popup,
                      KigDocument& doc, KigWidget& w, NormalMode& m );
};

class ObjectConstructorActionsProvider
  : public PopupActionProvider
{
  std::vector<ObjectConstructor*> mctors[NormalModePopupObjects::NumberOfMenus];
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const Objects& os,
                      NormalModePopupObjects& popup,
                      KigDocument& doc, KigWidget& w, NormalMode& m );
};

class PropertiesActionsProvider
  : public PopupActionProvider
{
  std::vector<int> mprops[3];
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const Objects& os,
                      NormalModePopupObjects& popup,
                      KigDocument& doc, KigWidget& w, NormalMode& m );
};

class ObjectTypeActionsProvider
  : public PopupActionProvider
{
  int mnoa;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const Objects& os,
                      NormalModePopupObjects& popup,
                      KigDocument& doc, KigWidget& w, NormalMode& m );
};

NormalModePopupObjects::NormalModePopupObjects( KigDocument& doc,
                                                KigWidget& view,
                                                NormalMode& mode,
                                                const Objects& objs )
  : KPopupMenu( &view ), mplc( QCursor::pos() ), mdoc( doc ), mview( view ), mobjs( objs ),
    mmode( mode )
{
  bool empty = objs.empty();
  bool single = objs.size() == 1;
  connect( this, SIGNAL( activated( int ) ), this, SLOT( toplevelMenuSlot( int ) ) );

  insertTitle( empty ? i18n( "Kig Document" )
               : single ? ObjectImp::translatedName( objs[0]->imp()->id() )
               : i18n( "%1 Objects" ).arg( objs.size() ), 1 );

  if ( empty )
  {
    // provides some diverse stuff like "unhide all", set coordinate
    // system etc.
    mproviders.push_back( new BuiltinDocumentActionsProvider() );
  };
  // construct an object using these objects and start constructing an
  // object using these objects
  mproviders.push_back( new ObjectConstructorActionsProvider() );
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

  for ( int i = 0; i < (int) NumberOfMenus; ++i )
    mmenus[i] = new QPopupMenu( this );

  connect( mmenus[TransformMenu], SIGNAL( activated( int ) ),
           this, SLOT( transformMenuSlot( int ) ) );
  connect( mmenus[ConstructMenu], SIGNAL( activated( int ) ),
           this, SLOT( constructMenuSlot( int ) ) );
  connect( mmenus[StartMenu], SIGNAL( activated( int ) ),
           this, SLOT( startMenuSlot( int ) ) );
  connect( mmenus[ShowMenu], SIGNAL( activated( int ) ),
           this, SLOT( showMenuSlot( int ) ) );
  connect( mmenus[SetColorMenu], SIGNAL( activated( int ) ),
           this, SLOT( setColorMenuSlot( int ) ) );
  connect( mmenus[SetSizeMenu], SIGNAL( activated( int ) ),
           this, SLOT( setSizeMenuSlot( int ) ) );
  connect( mmenus[SetCoordinateSystemMenu], SIGNAL( activated( int ) ),
           this, SLOT( setCoordinateSystemMenuSlot( int ) ) );

  for ( int i = 0; i <= NumberOfMenus; ++i )
  {
    int nextfree = 10;
    for ( uint j = 0; j < mproviders.size(); ++j )
      mproviders[j]->fillUpMenu( *this, i, nextfree );
  };
  static const QString menunames[NumberOfMenus] =
    {
      i18n( "&Transform" ),
      i18n( "Const&ruct" ),
      i18n( "&Start" ),
      i18n( "Sho&w" ),
      i18n( "Set Co&lor" ),
      i18n( "Set Si&ze" ),
      QString::null,
      i18n( "Set Coordinate S&ystem" )
    };
  int index = 1;
  for ( int i = 0; i < NumberOfMenus; ++i )
  {
    if ( mmenus[i]->count() == 0 ) continue;
    insertItem( menunames[i], mmenus[i], i, index++ );
  };
}

void NormalModePopupObjects::transformMenuSlot( int i )
{
  activateAction( TransformMenu, i );
}

void NormalModePopupObjects::constructMenuSlot( int i )
{
  activateAction( ConstructMenu, i );
}

void NormalModePopupObjects::startMenuSlot( int i )
{
  activateAction( StartMenu, i );
}

void NormalModePopupObjects::showMenuSlot( int i )
{
  activateAction( ShowMenu, i );
}

void NormalModePopupObjects::toplevelMenuSlot( int i )
{
  activateAction( ToplevelMenu, i );
}

void NormalModePopupObjects::activateAction( int menu, int action )
{
  bool done = false;
  // we need action - 10 cause we called fillUpMenu with nextfree set
  // to 10 initially..
  action -= 10;
  for ( uint i = 0; ! done && i < mproviders.size(); ++i )
    done = mproviders[i]->executeAction( menu, action, mobjs, *this, mdoc, mview, mmode );
}

NormalModePopupObjects::~NormalModePopupObjects()
{
  delete_all ( mproviders.begin(), mproviders.end() );
}

static const int numberofcolors = 8;
static const QColor* colors[numberofcolors] =
{
  &Qt::blue,
  &Qt::black,
  &Qt::gray,
  &Qt::red,
  &Qt::green,
  &Qt::cyan,
  &Qt::yellow,
  &Qt::darkRed
};

void BuiltinObjectActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    KIconLoader* l = KGlobal::iconLoader();
    popup.addAction( menu, i18n( "&Hide" ), nextfree++ );
    QPixmap p = l->loadIcon( "move", KIcon::User );
    popup.addAction( menu, p, i18n( "&Move" ), nextfree++ );
    p = l->loadIcon( "editdelete", KIcon::Toolbar );
    popup.addAction( menu, p, i18n( "&Delete" ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::SetColorMenu )
  {
    QPixmap p( 50, 20 );
    for( const QColor** c = colors; c < colors + numberofcolors; ++c )
    {
      p.fill( **c );
      popup.addAction( menu, p, nextfree++ );
    }
  }
  else if ( menu == NormalModePopupObjects::SetSizeMenu )
  {
    bool point = true;
    bool samecolor = true;
    Objects os = popup.objects();
    QColor color = os.front()->color();
    for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( ! (*i)->imp()->inherits( ObjectImp::ID_PointImp ) )
        point = false;
      if ( (*i)->color() != color ) samecolor = false;
    };
    if ( ! samecolor ) color = Qt::blue;
    QPixmap p( point ? 20 : 50, 20 );
    for ( int i = 1; i < 8; ++i )
    {
      p.fill( popup.eraseColor() );
      QPainter ptr( &p );
      ptr.setPen( QPen( color, 1 ) );
      ptr.setBrush( QBrush( color, Qt::SolidPattern ) );
      if ( point )
      {
        int size = 2*i;
        QRect r( ( 20 - size ) / 2,  ( 20 - size ) / 2, size, size );
        ptr.drawEllipse( r );
      }
      else
      {
        ptr.setPen( QPen( color, -1 + 2*i ) );
        ptr.drawLine( QPoint( 0, 10 ), QPoint( 50, 10 ) );
      };
      ptr.end();
      popup.addAction( menu, p, nextfree++ );
    };
  }
}

bool BuiltinObjectActionsProvider::executeAction(
  int menu, int& id, const Objects& os, NormalModePopupObjects& popup,
  KigDocument& doc, KigWidget& w, NormalMode& mode )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id > 2 )
    {
      id -= 3;
      return false;
    };
    switch( id )
    {
    case 0:
      // hide the objects..
      for_each( os.begin(), os.end(),
                bind2nd( mem_fun( &Object::setShown ), false ) );
      doc.mode()->redrawScreen();
      break;
    case 1:
    {
      // move
      QCursor::setPos( popup.mapToGlobal( QPoint( 0, 0 ) ) );
      QPoint p = w.mapFromGlobal( QCursor::pos() );
      Coordinate c = w.fromScreen( p );
      MovingMode m( os, c, w, doc );
      doc.runMode( &m );
      // in this case, we return, cause we don't want objects to be
      // unselected... ( or maybe we do ? )
      return true;
    }
    case 2:
      // delete
      doc.delObjects( os );
      break;
    default: assert( false );
    };
    mode.clearSelection();
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetColorMenu )
  {
    if ( id >= numberofcolors )
    {
      id -= numberofcolors;
      return false;
    };
    const QColor* color = colors[id];
    assert( color );
    for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
      (*i)->setColor( *color );
    mode.clearSelection();
    w.redrawScreen();
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetSizeMenu )
  {
    if ( id >= 7 )
    {
      id -= 7;
      return false;
    };

    for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
      if ( (*i)->inherits( Object::ID_RealObject ) )
        static_cast<RealObject*>(*i)->setWidth( 1 + 2*id );
    mode.clearSelection();
    w.redrawScreen();
    return true;
  }
  else return false;
}

void ObjectConstructorActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  const KigDocument& d = popup.document();
  const KigWidget& v = popup.widget();
  typedef ObjectConstructorList::vectype vectype;
  vectype vec = ObjectConstructorList::instance()->constructors();

  for ( vectype::iterator i = vec.begin(); i != vec.end(); ++i )
  {
    bool add = false;
    if ( popup.objects().empty() )
    {
      add = menu == NormalModePopupObjects::StartMenu && ! (*i)->isTransform();
    }
    else
    {
      int ret = (*i)->wantArgs( popup.objects(), d, v );
      if ( ret == ArgsChecker::Invalid ) continue;
      if ( (*i)->isTransform() && popup.objects().size() == 1 ) add = menu == NormalModePopupObjects::TransformMenu;
      else if ( ( *i )->isIntersection() ) add = menu == NormalModePopupObjects::ToplevelMenu;
      else if ( ret == ArgsChecker::Complete ) add = menu == NormalModePopupObjects::ConstructMenu;
      else add = menu == NormalModePopupObjects::StartMenu;
    };
    if ( add )
    {
      QCString iconfile = (*i)->iconFileName();
      if ( !iconfile.isEmpty() && !iconfile.isNull() )
      {
        QPixmap icon = KGlobal::iconLoader()->loadIcon( iconfile, KIcon::User );
        popup.addAction( menu, icon, (*i)->descriptiveName(), nextfree++ );
      }
      else
        popup.addAction( menu, (*i)->descriptiveName(), nextfree++ );
      mctors[menu].push_back( *i );
    }
  };
}

bool ObjectConstructorActionsProvider::executeAction(
  int menu, int& id, const Objects& os,
  NormalModePopupObjects&,
  KigDocument& doc, KigWidget& w, NormalMode& m )
{
  if ( (uint) id >= mctors[menu].size() )
  {
    id -= mctors[menu].size();
    return false;
  }

  ObjectConstructor* ctor = mctors[menu][id];
  if ( ! os.empty() && ctor->wantArgs( os, doc, w ) == ArgsChecker::Complete )
  {
    ctor->handleArgs( os, doc, w );
    m.clearSelection();
    w.redrawScreen();
  }
  else
  {
    ConstructMode m( doc, ctor );
    m.selectObjects( os, w );
    doc.runMode( &m );
  };
  return true;
}

void NormalModePopupObjects::addAction( int menu, const QPixmap& pix, int id )
{
  QPopupMenu* m = 0;
  if ( menu == ToplevelMenu ) m = this;
  else m = mmenus[menu];
  int ret = m->insertItem( pix, id );
  assert( ret == id );
}

void NormalModePopupObjects::setColorMenuSlot( int i )
{
  activateAction( SetColorMenu, i );
}

void NormalModePopupObjects::setSizeMenuSlot( int i )
{
  activateAction( SetSizeMenu, i );
}

void NormalModePopupObjects::setCoordinateSystemMenuSlot( int i )
{
  activateAction( SetCoordinateSystemMenu, i );
}

void NormalModePopupObjects::addAction( int menu, const QPixmap& icon, const QString& name, int id )
{
  QPopupMenu* m = 0;
  if ( menu == ToplevelMenu ) m = this;
  else m = mmenus[menu];
  int ret = m->insertItem( QIconSet( icon ), name, id );
  assert( ret == id );
}

void NormalModePopupObjects::addAction( int menu, const QString& name, int id )
{
  QPopupMenu* m = 0;
  if ( menu == ToplevelMenu ) m = this;
  else m = mmenus[menu];
  int ret = m->insertItem( name, id );
  assert( ret == id );
}

PopupActionProvider::~PopupActionProvider()
{
}

void PropertiesActionsProvider::fillUpMenu( NormalModePopupObjects& popup,
                                            int menu, int& nextfree )
{
  if ( popup.objects().size() != 1 ) return;
  Object* o = popup.objects()[0];
  uint np = o->numberOfProperties();
  if ( menu != NormalModePopupObjects::ConstructMenu &&
       menu != NormalModePopupObjects::ShowMenu ) return;
  for ( uint i = 0; i < np; ++i )
  {
    ObjectImp* prop = o->property( i, popup.document() );
    const char* iconfile = o->iconForProperty( i );
    bool add = true;
    if ( menu == NormalModePopupObjects::ConstructMenu )
    {
      // we don't want imp's like DoubleImp, since we can't show them
      // anyway..
      add &= ! prop->inherits( ObjectImp::ID_BogusImp );
      // we don't want to construct PointImp's coordinate property,
      // since it would construct a point at the same place as its
      // parent..
      add &= ! ( o->hasimp( ObjectImp::ID_PointImp ) &&
                 prop->inherits( ObjectImp::ID_PointImp ) );
    }
    else if ( menu == NormalModePopupObjects::ShowMenu )
      add &= prop->canFillInNextEscape();
    if ( add )
    {
      if ( iconfile && *iconfile )
      {
        QPixmap pix = KGlobal::iconLoader()->loadIcon( iconfile, KIcon::User );
        popup.addAction( menu, pix, i18n( o->properties()[i] ), nextfree++ );
      }
      else
      {
        popup.addAction( menu, i18n( o->properties()[i] ), nextfree++ );
      };
      mprops[menu-1].push_back( i );
    };
    delete prop;
  };
}

bool PropertiesActionsProvider::executeAction(
  int menu, int& id, const Objects& os,
  NormalModePopupObjects& popup,
  KigDocument& doc, KigWidget& w, NormalMode& )
{
  if ( menu != NormalModePopupObjects::ConstructMenu &&
       menu != NormalModePopupObjects::ShowMenu )
    return false;
  if ( (uint) id >= mprops[menu - 1].size() )
  {
    id -= mprops[menu - 1].size();
    return false;
  }
  int propid = mprops[menu-1][id];
  assert( os.size() == 1 );
  Object* parent = os[0];
  if ( menu == NormalModePopupObjects::ShowMenu )
  {
    Objects ret;
    ret.push_back( new PropertyObject( parent, propid ) );
    Coordinate c = w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) );
    Objects labelos = ObjectFactory::instance()->label(
      QString::fromLatin1( "%1" ), c,
      false, ret );
    copy( labelos.begin(), labelos.end(), back_inserter( ret ) );
    ret.calc( doc );
    doc.addObjects( ret );
  }
  else
  {
    Objects ret;
    ret.push_back( new PropertyObject( parent, propid ) );
    ret.push_back( new RealObject( CopyObjectType::instance(), ret ) );
    doc.addObjects( ret );
  };
  return true;
}

void ObjectTypeActionsProvider::fillUpMenu(
  NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( popup.objects().size() != 1 ) return;
  if ( menu != NormalModePopupObjects::ToplevelMenu ) return;
  Object* to = popup.objects()[0];
  if ( ! to->inherits( Object::ID_RealObject ) ) return;
  RealObject* o = static_cast<RealObject*>( to );
  const ObjectType* t = o->type();

  QStringList l = t->specialActions();
  mnoa = l.count();
  for ( int i = 0; i < mnoa; ++i )
    popup.addAction( menu, l[i], nextfree++ );
}

bool ObjectTypeActionsProvider::executeAction(
  int menu, int& id, const Objects& os,
  NormalModePopupObjects&,
  KigDocument& doc, KigWidget& w, NormalMode& m )
{
  if ( menu != NormalModePopupObjects::ToplevelMenu ) return false;
  if ( id >= mnoa )
  {
    id -= mnoa;
    return false;
  }
  assert( os.size() == 1 && os[0]->inherits( Object::ID_RealObject ) );
  RealObject* o = static_cast<RealObject*>( os[0] );
  o->type()->executeAction( id, o, doc, w, m );
  return true;
}

void BuiltinDocumentActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    popup.addAction( menu, i18n( "Unhide &All" ), nextfree++ );
    KIconLoader* l = KGlobal::iconLoader();
    QPixmap p = l->loadIcon( "viewmag+", KIcon::Toolbar );
    popup.addAction( menu, p, i18n( "Zoom &In" ), nextfree++ );
    p = l->loadIcon( "viewmag-", KIcon::Toolbar );
    popup.addAction( menu, p, i18n( "Zoom &Out" ), nextfree++ );
    if ( popup.widget().isFullScreen() )
      popup.addAction( menu, i18n( "E&xit Full Screen Mode" ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::SetCoordinateSystemMenu )
  {
    QStringList l = CoordinateSystemFactory::names();
    mnumberofcoordsystems = l.count();
    for ( uint i = 0; i < l.count(); ++i )
      popup.addAction( menu, l[i], nextfree++ );
  }
}

bool BuiltinDocumentActionsProvider::executeAction(
  int menu, int& id, const Objects&,
  NormalModePopupObjects& popup,
  KigDocument& doc, KigWidget& w, NormalMode& m )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id == 0 )
    {
      doc.showHidden();
      m.clearSelection();
      w.redrawScreen();
      return true;
    }
    else if ( id == 1 )
    {
      w.zoomIn();
      return true;
    }
    else if ( id == 2 )
    {
      w.zoomOut();
      return true;
    }
    else if ( popup.widget().isFullScreen() )
    {
      if ( id == 3 )
      {
        assert( w.view()->parent()->inherits( "QDialog" ) );
        static_cast<QDialog*>( w.view()->parent() )->close();
        return true;
      }
      else
        id -= 1;
    };
    id -= 3;
    return false;
  }
  else if ( menu == NormalModePopupObjects::SetCoordinateSystemMenu )
  {
    if ( id >= mnumberofcoordsystems )
    {
      id -= mnumberofcoordsystems;
      return false;
    };
    CoordinateSystem* sys = CoordinateSystemFactory::build( id );
    assert( sys );
    doc.history()->addCommand( KigCommand::changeCoordSystemCommand( doc, sys ) );
    m.clearSelection();
    return true;
  }
  else return false;
}

