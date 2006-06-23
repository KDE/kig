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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "popup.h"
#include "popup.moc"

#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"
#include "../objects/object_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/bogus_imp.h"
#include "../objects/point_imp.h"
#include "../objects/line_imp.h"
#include "../objects/other_type.h"
#include "../objects/object_factory.h"
#include "../objects/polygon_imp.h"
#include "../objects/text_imp.h"
#include "../misc/lists.h"
#include "../misc/argsparser.h"
#include "../misc/kigpainter.h"
#include "../misc/coordinate_system.h"
#include "../misc/object_constructor.h"
#include "construct_mode.h"
#include "normal.h"
#include "moving.h"

#include <algorithm>
#include <functional>

#include <qcursor.h>
#include <qdialog.h>
#include <qpen.h>
#include <qregexp.h>
#include <qvalidator.h>

#include <kaction.h>
#include <kcolordialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#if KDE_IS_VERSION( 3, 1, 90 )
#include <kinputdialog.h>
#else
#include <klineeditdlg.h>
#endif

#include <config.h>

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
  virtual bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                              NormalModePopupObjects& popup,
                              KigPart& doc, KigWidget& w, NormalMode& m ) = 0;
};

class BuiltinObjectActionsProvider
  : public PopupActionProvider
{
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

class NameObjectActionsProvider
  : public PopupActionProvider
{
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

class BuiltinDocumentActionsProvider
  : public PopupActionProvider
{
  int mnumberofcoordsystems;
  bool misfullscreen;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

class ObjectConstructorActionsProvider
  : public PopupActionProvider
{
  std::vector<ObjectConstructor*> mctors[NormalModePopupObjects::NumberOfMenus];
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

class PropertiesActionsProvider
  : public PopupActionProvider
{
  // we don't really need NumberOfMenus vectors, but this is the
  // easiest way to do it, and I'm too lazy to do it properly ;)
  std::vector<int> mprops[NormalModePopupObjects::NumberOfMenus];
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

class ObjectTypeActionsProvider
  : public PopupActionProvider
{
  int mnoa;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/script-common.h"
#include "../scripting/script_mode.h"
#include "../scripting/python_type.h"

class ScriptActionsProvider
  : public PopupActionProvider
{
  int mns;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};
#endif

NormalModePopupObjects::NormalModePopupObjects( KigPart& part,
                                                KigWidget& view,
                                                NormalMode& mode,
                                                const std::vector<ObjectHolder*>& objs,
						const QPoint& plc )
  : KPopupMenu( &view ), mplc( plc ), mpart( part ), mview( view ), mobjs( objs ),
    mmode( mode ), monlylabels( false )
{
  bool empty = objs.empty();
  bool single = objs.size() == 1;
  connect( this, SIGNAL( activated( int ) ), this, SLOT( toplevelMenuSlot( int ) ) );

  QString title;
  if ( empty )
    title = i18n( "Kig Document" );
  else if ( single )
  {
    if ( !objs[0]->name().isNull() )
      title = QString::fromLatin1( "%1 %2" ).arg( objs[0]->imp()->type()->translatedName() ).arg( objs[0]->name() );
    else
      title = objs[0]->imp()->type()->translatedName();
  }
  else
    title = i18n( "%1 Objects" ).arg( objs.size() );
  insertTitle( title, 1 );

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

  for ( uint i = 0; i < NumberOfMenus; ++i )
    mmenus[i] = new QPopupMenu( this );

  connect( mmenus[TransformMenu], SIGNAL( activated( int ) ),
           this, SLOT( transformMenuSlot( int ) ) );
  connect( mmenus[TestMenu], SIGNAL( activated( int ) ),
           this, SLOT( testMenuSlot( int ) ) );
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
  connect( mmenus[SetStyleMenu], SIGNAL( activated( int ) ),
           this, SLOT( setStyleMenuSlot( int ) ) );
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
      i18n( "T&est" ),
      i18n( "Const&ruct" ),
      i18n( "&Start" ),
      i18n( "Add Te&xt Label" ),
      i18n( "Set Co&lor" ),
      i18n( "Set &Pen Width" ),
      i18n( "Set St&yle" ),
      QString::null,
      i18n( "Set Coordinate S&ystem" )
    };
  static const QString menuicons[NumberOfMenus] =
    {
      "centralsymmetry",
      "test",
      QString::null,
      "launch",
      "kig_text",
      "color_fill",
//      "colorize",
      "sizer",
      "paintbrush",
      QString::null,
      QString::null
    };
  int index = 1;
  for ( int i = 0; i < NumberOfMenus; ++i )
  {
    if ( mmenus[i]->count() == 0 ) continue;
    if ( menuicons[i].isNull() )
      insertItem( menunames[i], mmenus[i], i, index++ );
    else
    {
      KIconLoader* l = part.instance()->iconLoader();
      QPixmap icon = l->loadIcon( menuicons[i], KIcon::Small, 22, KIcon::DefaultState, 0L, true );
      insertItem( QIconSet( icon ), menunames[i], mmenus[i], i, index++ );
    }
  };
}

void NormalModePopupObjects::testMenuSlot( int i )
{
  activateAction( TestMenu, i );
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
    done = mproviders[i]->executeAction( menu, action, mobjs, *this, mpart, mview, mmode );
}

NormalModePopupObjects::~NormalModePopupObjects()
{
  delete_all ( mproviders.begin(), mproviders.end() );
}

static const QColor* colors[] =
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
static const int numberofcolors = sizeof( colors ) / sizeof( QColor* );

void BuiltinObjectActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    KIconLoader* l = popup.part().instance()->iconLoader();
    std::vector<ObjectHolder*> os = popup.objects();

    /*
     * mp: we want the "show" action to be visible only
     * if we selected only one object (to be conservative)
     * and if that object is currently hidden.
     * conversely for one hidden object we don't want
     * the "hide" action to be inserted.
     * in any case we have a fixed 'id' associated
     * with the two actions.
     */

    if ( os.size() > 1 || os.front()->shown() )
    {
      popup.addAction( menu, i18n( "&Hide" ), nextfree );
    }
    if ( os.size() == 1 && !os.front()->shown() )
    {
      popup.addAction( menu, i18n( "&Show" ), nextfree+1 );
    }
    nextfree += 2;
    QPixmap p = l->loadIcon( "move", KIcon::Toolbar );
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
    popup.addAction( menu, i18n( "&Custom Color" ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::SetSizeMenu && !popup.onlyLabels() )
  {
    bool point = true;
    bool samecolor = true;
    std::vector<ObjectHolder*> os = popup.objects();
    QColor color = os.front()->drawer()->color();
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( ! (*i)->imp()->inherits( PointImp::stype() ) )
        point = false;
      if ( (*i)->drawer()->color() != color ) samecolor = false;
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
  else if ( menu == NormalModePopupObjects::SetStyleMenu && !popup.onlyLabels() )
  {
    bool samecolor = true;
    int npoints = 0;
    int nothers = 0;
    std::vector<ObjectHolder*> os = popup.objects();
    QColor color = os.front()->drawer()->color();
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( (*i)->imp()->inherits( PointImp::stype() ) )
        npoints++;
      else
        nothers++;
      if ( (*i)->drawer()->color() != color ) samecolor = false;
    };
    bool point = ( npoints > nothers );
    if ( ! samecolor ) color = Qt::blue;
    if ( point )
      for ( int i = 0; i < 5; ++i )
      {
        QPixmap p( 20, 20 );
        p.fill( popup.eraseColor() );
        ScreenInfo si( Rect( -1, -1, 2, 2 ), p.rect() );
        KigPainter ptr( si, &p, popup.part().document(), false );
        PointImp pt( Coordinate( 0, 0 ) );
        ObjectDrawer d( color, -1, true, Qt::SolidLine, i );
        d.draw( pt, ptr, false );
        popup.addAction( menu, p, nextfree++ );
      }
    else
    {
      Qt::PenStyle penstyles[] = {Qt::SolidLine, Qt::DashLine, Qt::DashDotLine, Qt::DashDotDotLine, Qt::DotLine};
      for ( int i = 0; i < (int) ( sizeof( penstyles ) / sizeof( Qt::PenStyle ) ); ++i )
      {
        QPixmap p( 50, 20 );
        p.fill( popup.eraseColor() );
        ScreenInfo si( Rect( -2.5, -1, 5, 2 ), p.rect() );
        KigPainter ptr( si, &p, popup.part().document(), false );
        LineImp line( Coordinate( -1, 0 ), Coordinate( 1, 0 ) );
        Qt::PenStyle ps = penstyles[i];
        ObjectDrawer d( color, -1, true, ps, 1 );
        d.draw( line, ptr, false );
        popup.addAction( menu, p, nextfree++ );
      };
    }
  }
}

void NameObjectActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    popup.addAction( menu, i18n( "Set &Name..." ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::ShowMenu )
  {
    popup.addAction( menu, i18n( "&Name" ), nextfree++ );
  }
}

static void addNameLabel( ObjectCalcer* object, ObjectCalcer* namecalcer, const Coordinate& loc, KigPart& doc )
{
  std::vector<ObjectCalcer*> args;
  args.push_back( namecalcer );
  const bool namelabelneedsframe = false;
  ObjectCalcer* attachto = 0;
  if ( object->imp()->inherits( PointImp::stype() ) || 
       object->imp()->attachPoint().valid() || 
       object->imp()->inherits( CurveImp::stype() ) )
    attachto = object;
  ObjectHolder* label = ObjectFactory::instance()->attachedLabel(
      QString::fromLatin1( "%1" ), attachto, loc, namelabelneedsframe, args, doc.document() );
  doc.addObject( label );
}

bool NameObjectActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os, NormalModePopupObjects& popup,
  KigPart& doc, KigWidget& w, NormalMode& )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id >= 1 )
    {
      id -= 1;
      return false;
    }
    assert( os.size() == 1 );
    QString name = os[0]->name();
    bool ok;
    QRegExp re(  ".*" );
    QRegExpValidator* rev = new QRegExpValidator(  re,  &doc );
    QString caption = i18n( "Set Object Name" );
    QString label = i18n( "Set Name of this Object:" );
#if KDE_IS_VERSION( 3, 1, 90 )
    name = KInputDialog::getText( caption, label, name, &ok, &w, 0, rev );
#else
    name = KLineEditDlg::getText( caption, label, name, &ok, &w, rev );
#endif
    if ( ok )
    {
      bool justadded = false;
      ObjectCalcer* namecalcer = os[0]->nameCalcer();
      if ( !namecalcer )
      {
        justadded = true;
        ObjectConstCalcer* c = new ObjectConstCalcer( new StringImp( i18n( "<unnamed object>" ) ) );
        os[0]->setNameCalcer( c );
        namecalcer = c;
      }
      assert( dynamic_cast<ObjectConstCalcer*>( namecalcer ) );
      ObjectConstCalcer* cnamecalcer = static_cast<ObjectConstCalcer*>( os[0]->nameCalcer() );
      MonitorDataObjects mon( cnamecalcer );
      cnamecalcer->setImp( new StringImp( name ) );
      KigCommand* kc = new KigCommand( doc, i18n( "Set Object Name" ) );
      mon.finish( kc );
      doc.history()->addCommand( kc );

      // if we just added the name, we add a label to show it to the user.
      if ( justadded )
        addNameLabel( os[0]->calcer(), namecalcer,
//                    w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) ),
                      w.fromScreen( popup.plc() ),
                      doc );
    }
    return true;
  }
  else if ( menu == NormalModePopupObjects::ShowMenu )
  {
    if ( id >= 1 )
    {
      id -= 1;
      return false;
    }
    assert( os.size() == 1 );
    ObjectCalcer* namecalcer = os[0]->nameCalcer();
    if ( !namecalcer )
    {
      ObjectConstCalcer* c = new ObjectConstCalcer( new StringImp( i18n( "<unnamed object>" ) ) );
      os[0]->setNameCalcer( c );
      namecalcer = c;
    }
    addNameLabel( os[0]->calcer(), namecalcer,
//                  w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) ), doc );
                  w.fromScreen( popup.plc() ), doc );
    return true;
  }
  else
  {
    return false;
  }
}

bool BuiltinObjectActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os, NormalModePopupObjects& popup,
  KigPart& doc, KigWidget& w, NormalMode& mode )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id > 3 )
    {
      id -= 4;
      return false;
    };
    switch( id )
    {
    case 0:
      // hide the objects..
      doc.hideObjects( os );
      break;
    case 1:
      // show the objects..
      doc.showObjects( os );
      break;
    case 2:
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
    case 3:
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
    if ( id >= numberofcolors + 1 )
    {
      id -= numberofcolors + 1;
      return false;
    };
    QColor color;
    if ( id < numberofcolors )
      color = *colors[id];
    else
    {
      if ( os.size() == 1 )
        color = os.front()->drawer()->color();
      int result = KColorDialog::getColor( color, &w );
      if ( result != KColorDialog::Accepted ) return true;
    }
    KigCommand* kc = new KigCommand( doc, i18n( "Change Object Color" ) );
    assert( color.isValid() );
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
      kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyColor( color ) ) );
    doc.history()->addCommand( kc );
    mode.clearSelection();
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetSizeMenu )
  {
    if ( id >= 7 )
    {
      id -= 7;
      return false;
    };

    KigCommand* kc = new KigCommand( doc, i18n( "Change Object Width" ) );
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
      kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyWidth( 1 + 2 * id ) ) );
    doc.history()->addCommand( kc );
    mode.clearSelection();
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetStyleMenu )
  {
    int npoints = 0;
    int nothers = 0;
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( (*i)->imp()->inherits( PointImp::stype() ) )
        npoints++;
      else
        nothers++;
    };
    bool point = ( npoints > nothers );
    int max = point ? 5 : 5;
    if ( id >= max )
    {
      id -= max;
      return false;
    };

    if ( point )
    {
      KigCommand* kc = new KigCommand( doc, i18n( "Change Point Style" ) );
      for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
        if ( (*i)->imp()->inherits( PointImp::stype() ) )
          kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyPointStyle( id ) ) );
      doc.history()->addCommand( kc );
      mode.clearSelection();
      return true;
    }
    else
    {
      Qt::PenStyle penstyles[] = {Qt::SolidLine, Qt::DashLine, Qt::DashDotLine, Qt::DashDotDotLine, Qt::DotLine};
      assert( id < (int)( sizeof( penstyles ) / sizeof( Qt::PenStyle ) ) );
      Qt::PenStyle p = penstyles[id];
      KigCommand* kc = new KigCommand( doc, i18n( "Change Object Style" ) );
      for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
        if ( ! (*i)->imp()->inherits( PointImp::stype() ) )
          kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyStyle( p ) ) );
      doc.history()->addCommand( kc );
      mode.clearSelection();
    }
    return true;
  }
  else return false;
}

void ObjectConstructorActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  const KigDocument& d = popup.part().document();
  const KigWidget& v = popup.widget();
  typedef ObjectConstructorList::vectype vectype;
  vectype vec = ObjectConstructorList::instance()->constructors();

  for ( vectype::iterator i = vec.begin(); i != vec.end(); ++i )
  {
    bool add = false;
    if ( popup.objects().empty() )
    {
      add = menu == NormalModePopupObjects::StartMenu && ! (*i)->isTransform() && ! (*i)->isTest();
    }
    else
    {
      int ret = (*i)->wantArgs( getCalcers( popup.objects() ), d, v );
      if ( ret == ArgsParser::Invalid ) continue;
      if ( (*i)->isTransform() && popup.objects().size() == 1 ) add = menu == NormalModePopupObjects::TransformMenu;
      else if ( (*i)->isTest() ) add = menu == NormalModePopupObjects::TestMenu;
      else if ( ( *i )->isIntersection() ) add = menu == NormalModePopupObjects::ToplevelMenu;
      else if ( ret == ArgsParser::Complete ) add = menu == NormalModePopupObjects::ConstructMenu;
      else add = menu == NormalModePopupObjects::StartMenu;
    };
    if ( add )
    {
      QCString iconfile = (*i)->iconFileName();
      if ( !iconfile.isEmpty() && !iconfile.isNull() )
      {
        QPixmap icon = popup.part().instance()->iconLoader()->loadIcon( iconfile, KIcon::Toolbar, 22, KIcon::DefaultState, 0L, true );
        popup.addAction( menu, icon, (*i)->descriptiveName(), nextfree++ );
      }
      else
        popup.addAction( menu, (*i)->descriptiveName(), nextfree++ );
      mctors[menu].push_back( *i );
    }
  };
}

bool ObjectConstructorActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects&,
  KigPart& doc, KigWidget& w, NormalMode& m )
{
  if ( (uint) id >= mctors[menu].size() )
  {
    id -= mctors[menu].size();
    return false;
  }

  ObjectConstructor* ctor = mctors[menu][id];
  std::vector<ObjectCalcer*> osc = getCalcers( os );
  if ( ! os.empty() && ctor->wantArgs( osc, doc.document(), w ) == ArgsParser::Complete )
  {
    ctor->handleArgs( osc, doc, w );
    m.clearSelection();
  }
  else
  {
    BaseConstructMode* mode = ctor->constructMode( doc );
    mode->selectObjects( os, w );
    doc.runMode( mode );
    delete mode;
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
  // pretend to use this var..
  (void) ret;
}

void NormalModePopupObjects::setColorMenuSlot( int i )
{
  activateAction( SetColorMenu, i );
}

void NormalModePopupObjects::setSizeMenuSlot( int i )
{
  activateAction( SetSizeMenu, i );
}

void NormalModePopupObjects::setStyleMenuSlot( int i )
{
  activateAction( SetStyleMenu, i );
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
  // pretend to use this var..
  (void)ret;
}

void NormalModePopupObjects::addAction( int menu, const QString& name, int id )
{
  QPopupMenu* m = 0;
  if ( menu == ToplevelMenu ) m = this;
  else m = mmenus[menu];
  int ret = m->insertItem( name, id );
  assert( ret == id );
  // pretend to use this var..
  (void)ret;
}

PopupActionProvider::~PopupActionProvider()
{
}

void PropertiesActionsProvider::fillUpMenu( NormalModePopupObjects& popup,
                                            int menu, int& nextfree )
{
  if ( popup.objects().size() != 1 ) return;
  ObjectHolder* o = popup.objects()[0];
  uint np = o->imp()->numberOfProperties();
  if ( menu != NormalModePopupObjects::ConstructMenu &&
       menu != NormalModePopupObjects::ShowMenu ) return;
  for ( uint i = 0; i < np; ++i )
  {
    ObjectImp* prop = o->imp()->property( i, popup.part().document() );
    const char* iconfile = o->imp()->iconForProperty( i );
    bool add = true;
    if ( menu == NormalModePopupObjects::ConstructMenu )
    {
      // we don't want imp's like DoubleImp, since we can't show them
      // anyway..
      add &= ! prop->inherits( BogusImp::stype() );
      // we don't want to construct PointImp's coordinate property,
      // since it would construct a point at the same place as its
      // parent..
      add &= ! ( o->imp()->inherits( PointImp::stype() ) &&
                 prop->inherits( PointImp::stype() ) );
    }
    else if ( menu == NormalModePopupObjects::ShowMenu )
      add &= prop->canFillInNextEscape();
    if ( add )
    {
      if ( iconfile && *iconfile )
      {
        QPixmap pix = popup.part().instance()->iconLoader()->loadIcon( iconfile, KIcon::Toolbar, 22, KIcon::DefaultState, 0L, true );
        popup.addAction( menu, pix, i18n( o->imp()->properties()[i] ), nextfree++ );
      }
      else
      {
        popup.addAction( menu, i18n( o->imp()->properties()[i] ), nextfree++ );
      };
      mprops[menu-1].push_back( i );
    };
    delete prop;
  };
}

bool PropertiesActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects& popup,
  KigPart& doc, KigWidget& w, NormalMode& )
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
  ObjectHolder* parent = os[0];
  if ( menu == NormalModePopupObjects::ShowMenu )
  {
    std::vector<ObjectCalcer*> args;
    args.push_back( new ObjectPropertyCalcer( parent->calcer(), propid ) );
    args.back()->calc( doc.document() );
// TODO: recover the cursor position somehow... the following does not work
// in general...
//    Coordinate c = w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) );
// mp: it seems that we have no idea where to position the label, 
// btw what's the meaning of (5,0)?    let the
// attach method decide what to do... (passing an invalidCoord)
//  ///////    Coordinate c = Coordinate::invalidCoord();
    Coordinate c = w.fromScreen( popup.plc() );
    ObjectHolder* label = ObjectFactory::instance()->attachedLabel(
      QString::fromLatin1( "%1" ), parent->calcer(), c,
      false, args, doc.document() );
    doc.addObject( label );
  }
  else
  {
    ObjectHolder* h = new ObjectHolder(
      new ObjectPropertyCalcer( parent->calcer(), propid ) );
    h->calc( doc.document() );
    doc.addObject( h );
  };
  return true;
}

void ObjectTypeActionsProvider::fillUpMenu(
  NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( popup.objects().size() != 1 ) return;
  if ( menu != NormalModePopupObjects::ToplevelMenu ) return;
  ObjectHolder* to = popup.objects()[0];
  ObjectTypeCalcer* c = dynamic_cast<ObjectTypeCalcer*>( to->calcer() );
  if ( ! c ) return;
  const ObjectType* t = c->type();

  QStringList l = t->specialActions();
  mnoa = l.count();
  for ( int i = 0; i < mnoa; ++i )
    popup.addAction( menu, l[i], nextfree++ );
}

bool ObjectTypeActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects&,
  KigPart& doc, KigWidget& w, NormalMode& m )
{
  if ( menu != NormalModePopupObjects::ToplevelMenu ) return false;
  if ( id >= mnoa )
  {
    id -= mnoa;
    return false;
  }
  assert( os.size() == 1 );
  ObjectTypeCalcer* oc = dynamic_cast<ObjectTypeCalcer*>( os[0]->calcer() );
  assert(  oc );

  oc->type()->executeAction( id, *os[0], *oc, doc, w, m );
  return true;
}

void BuiltinDocumentActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    popup.addAction( menu, i18n( "U&nhide All" ), nextfree++ );
    popup.part().action( "view_zoom_in" )->plug( &popup );
    popup.part().action( "view_zoom_out" )->plug( &popup );
    popup.part().action( "fullscreen" )->plug( &popup );
    nextfree += 3;
  }
  else if ( menu == NormalModePopupObjects::SetCoordinateSystemMenu )
  {
    int idoffset = nextfree;
    QStringList l = CoordinateSystemFactory::names();
    mnumberofcoordsystems = l.count();
    for ( uint i = 0; i < l.count(); ++i )
      popup.addAction( menu, l[i], nextfree++ );
    int current = popup.part().document().coordinateSystem().id();
    popup.setChecked( menu, idoffset + current, true );
  }
}

bool BuiltinDocumentActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>&,
  NormalModePopupObjects&,
  KigPart& doc, KigWidget&, NormalMode& m )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    kdDebug() << "id: " << id << endl;
    if ( id == 0 )
    {
      doc.showHidden();
      m.clearSelection();
      return true;
    }
    id -= 1;
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

void NormalModePopupObjects::setChecked( int menu, int n, bool checked )
{
  mmenus[menu]->setItemChecked( n, checked );
}

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
/**
 * this is a local function that looks for a python script associated
 * to a clicked object
 */
static ObjectTypeCalcer* getPythonExecuteTypeFromCalcer( ObjectCalcer* o )
{
  ObjectTypeCalcer* oc = dynamic_cast<ObjectTypeCalcer *>( o );
  if ( !oc ) return 0;
  const PythonExecuteType* pythonexec = dynamic_cast<const PythonExecuteType*>( oc->type() );
  if ( pythonexec ) return oc;

  return 0;
}

void ScriptActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::StartMenu )
  {
    KIconLoader* l = popup.part().instance()->iconLoader();
    QPixmap p = l->loadIcon( ScriptType::icon( ScriptType::Python ), KIcon::Toolbar, 22, KIcon::DefaultState, 0L, true );
    popup.addAction( menu, p, i18n( "Python Script" ), nextfree++ );
    mns++;
  }
  else if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( !popup.objects().empty() &&
         getPythonExecuteTypeFromCalcer( popup.objects().front()->calcer() ) )
    {
      popup.addAction( menu, i18n( "Edit Script..." ), nextfree );
    }
    nextfree++;
  }
}

bool ScriptActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects&, KigPart& doc, KigWidget& w, NormalMode& mode )
{
  if ( menu == NormalModePopupObjects::StartMenu )
  {
    if ( id == 0 )
    {
      ScriptCreationMode m( doc );
      m.setScriptType( ScriptType::Python );
      if ( os.size() > 0 )
      {
        mode.clearSelection();
        m.addArgs( os, w );
        m.goToCodePage();
      }
      doc.runMode( &m );
      return true;
    }
    else
    {
      id -= mns;
    }
  }
  else if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id == 0 )
    {
      ObjectTypeCalcer* oc = getPythonExecuteTypeFromCalcer( os.front()->calcer() );
      if ( oc )
      {
        ScriptEditMode m( oc, doc );
        m.setScriptType( ScriptType::Python );
        doc.runMode( &m );
      }
      return true;
    }
    else
    {
      id -= 1;
    }
  }

  return false;
}
#endif

int ObjectChooserPopup::getObjectFromList( const QPoint& p, KigWidget* w,
                                           const std::vector<ObjectHolder*>& objs,
                                           bool givepopup )
{
  int size = objs.size();

  // no objects
  if ( size == 0 )
    return -1;

  int id = -1;

  int numpoints = 0;
  int numpolygons = 0;
  int numothers = 0;

  for ( std::vector<ObjectHolder*>::const_iterator i = objs.begin();
        i != objs.end(); ++i )
  {
    if ( (*i)->imp()->inherits( PointImp::stype() ) ) numpoints++;
    else if ( (*i)->imp()->inherits( PolygonImp::stype() ) ) numpolygons++;
    else numothers++;
  }

  // simply cases:
  // - only 1 point ( and eventually other objects )
  // - no points and an object which is not a polygon
  // - only one object
  // FIXME: we assume that our objects are sorted ( points, others, polygons )!
  if ( ( numpoints == 1 ) ||
       ( ( numpoints == 0 ) && ( numothers == 1 ) ) ||
       ( size == 1 ) )
    id = 0;
  else
  {
    if ( givepopup )
    {
      ObjectChooserPopup* ppp = new ObjectChooserPopup( p, *w, objs );
      ppp->exec( QCursor::pos() );

      id = ppp->mselected;

      delete ppp;
      ppp = 0;
    }
    else
    {
      // we don't want to show a popup to the user, so let's give a
      // value > 0 to indicate that it's not the first
      id = 1;
    }
  }
//  kdDebug() << "numpoints: " << numpoints << endl
//            << "numothers: " << numothers << endl
//            << "numpolygons: " << numpolygons << endl
//            << "id: " << id << endl;

  return id;
}

ObjectChooserPopup::ObjectChooserPopup( const QPoint& p, KigWidget& view,
                                        const std::vector<ObjectHolder*>& objs )
  : KPopupMenu(), mplc( p ), mview( view ), mobjs( objs ), mselected( -1 )
{
  for ( uint i = 0; i < mobjs.size(); i++ )
  {
    insertItem( !mobjs[i]->name().isEmpty()
                ? QString::fromLatin1( "%1 %2" ).arg( mobjs[i]->imp()->type()->translatedName() ).arg( mobjs[i]->name() )
                : mobjs[i]->imp()->type()->translatedName(),
                i );
  }

  connect( this, SIGNAL( activated( int ) ), this, SLOT( actionActivatedSlot( int ) ) );
}

ObjectChooserPopup::~ObjectChooserPopup()
{
}

void ObjectChooserPopup::actionActivatedSlot( int which )
{
  mselected = which;
}
