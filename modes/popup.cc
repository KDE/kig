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
#include "../misc/i18n.h"
#include "../objects/object_imp.h"
#include "../objects/object.h"
#include "construct_mode.h"
#include "normal.h"
#include "moving.h"

#include <qcursor.h>
#include <qpainter.h>
#include <qpen.h>

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

class BuiltinActionsProvider
  : public PopupActionProvider
{
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
  : mplc( QCursor::pos() ), mdoc( doc ), mview( view ), mobjs( objs ),
    mmode( mode )
{
  assert ( ! objs.empty() );
  bool single = objs.size() == 1;
  connect( this, SIGNAL( activated( int ) ), this, SLOT( toplevelMenuSlot( int ) ) );

  insertTitle( single ? ObjectImp::translatedName( objs[0]->imp()->id() )
               : i18n( "%1 Objects" ).arg( objs.size() ), 1 );

  mproviders.push_back( new BuiltinActionsProvider() );
//   mproviders.push_back( new ObjectConstructorActionsProvider() );
//   mproviders.push_back( new ConstructPropertiesActionsProvider() );
//   mproviders.push_back( new ShowPropertiesActionsProvider() );
//   mproviders.push_back( new ObjectTypeActionsProvider() );

  for ( int i = 0; i < 6; ++i )
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

  for ( int i = TransformMenu; i <= ToplevelMenu; ++i )
  {
    int nextfree = 2;
    for ( uint j = 0; j < mproviders.size(); ++j )
      mproviders[j]->fillUpMenu( *this, i, nextfree );
  };
  static const QString menunames[6] =
    {
      i18n( "Transform" ),
      i18n( "Construct" ),
      i18n( "Start" ),
      i18n( "Show" ),
      i18n( "Set Color" ),
      i18n( "Set Size" )
    };
  for ( int i = 0; i < 6; ++i )
  {
    if ( mmenus[i]->count() == 0 ) continue;
    insertItem( menunames[i], mmenus[i] );
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
  // we need action - 2 cause we called fillUpMenu with nextfree set
  // to 2 initially..
  action -= 2;
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
  &Qt::lightGray,
  &Qt::green,
  &Qt::cyan,
  &Qt::yellow,
  &Qt::darkRed
};

void BuiltinActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    popup.addAction( menu, i18n( "&Hide" ), nextfree++ );
    popup.addAction( menu, i18n( "&Move" ), nextfree++ );
    popup.addAction( menu, i18n( "&Delete" ), nextfree++ );
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
        int size = 1 + 2*i;
        QRect r( ( 20 - size ) / 2,  ( 20 - size ) / 2, size, size );
        ptr.drawEllipse( r );
      }
      else
      {
        ptr.setPen( QPen( color, i ) );
        ptr.drawLine( QPoint( 0, 10 ), QPoint( 50, 10 ) );
      };
      ptr.end();
      popup.addAction( menu, p, nextfree++ );
    };
  }
}

bool BuiltinActionsProvider::executeAction(
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
      doc.mode()->objectsRemoved();
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
        static_cast<RealObject*>(*i)->setWidth( 3 + 2*id );
    mode.clearSelection();
    w.redrawScreen();
  }
  return true;
}

void NormalModePopupObjects::addAction( int menu, const QString& name, int id )
{
  QPopupMenu* m = 0;
  if ( menu == ToplevelMenu ) m = this;
  else m = mmenus[menu];
  int ret = m->insertItem( name, id );
  assert( ret == id );
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
