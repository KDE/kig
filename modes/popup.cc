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
#include "../misc/type.h"
#include "../modes/constructing.h"
#include "normal.h"
#include "moving.h"

#include <qcursor.h>

NormalModePopupObjects::NormalModePopupObjects( KigDocument* doc,
                                                KigWidget* view,
                                                NormalMode* mode,
                                                const Objects& objs )
  : mplc( QCursor::pos() ), mdoc( doc ), mview( view ), mobjs( objs ), mmode( mode )
{
  assert ( ! objs.empty() );
  bool single = objs.size() == 1;
  connect( this, SIGNAL( activated( int ) ), this, SLOT( doAction( int ) ) );
  // title..
  insertTitle( single ? objs[0]->vTBaseTypeName()
               : i18n( "%1 Objects" ).arg( objs.size() ), titleId );

  addUsePopup();
  addColorPopup();
  addHideItem();
  addMoveItem();
  addDeleteItem();
  addVirtualItems();
}

void NormalModePopupObjects::doAction( int i )
{
  kdDebug() << k_funcinfo << " i == " << i << endl;
  assert( 1 < i );
  switch( i )
  {
  case hideId:
    // hide action..
    std::for_each( mobjs.begin(), mobjs.end(),
                   std::bind2nd( std::mem_fun( &Object::setShown ), false ) );
    mdoc->mode()->objectsRemoved();
    break;
  case deleteId:
    // delete action..
    mdoc->delObjects( mobjs );
    break;
  case moveId:
  {
    // move action..
    QRect r = this->frameRect();
    QPoint p = r.topLeft();
    p = this->mapToParent( p );
    Coordinate c = mview->fromScreen( p );
    MovingMode* m = new MovingMode( mobjs, c, mmode, mview, mdoc );
    mdoc->setMode( m );
    // in this case, we return, cause we don't want objects to be
    // unselected... ( or maybe we do ? )
    return;
  }
  default:
  {
    // i >= restOffset, which means that one of the object specific actions was
    // activated...
    assert( (uint) i >= virtualActionsOffset );
    assert( mobjs.size() == 1 );
    Object* o = mobjs[0];
    o->doNormalAction( i, mdoc, mview, mmode, mview->fromScreen( mview->mapFromGlobal( mplc ) ) );
  }
  };
  mmode->clearSelection();
}

const QColor* NormalModePopupObjects::color( int i )
{
  static const QColor* colors[] =
    { &Qt::blue,
      &Qt::black,
      &Qt::gray,
      &Qt::lightGray,
      &Qt::green,
      &Qt::cyan,
      &Qt::yellow,
      &Qt::darkRed,
      0
    };
  if( i < 0 || i > 8 ) return 0;
  return colors[i];
}

QPopupMenu* NormalModePopupObjects::colorMenu( QWidget* parent )
{
  QPopupMenu* m = new QPopupMenu( parent, "color popup menu" );
  const QColor* c = 0;
  QPixmap p( 50, 20 );
  for( uint i = 0; ( c = color( i ) ); ++i )
  {
    p.fill( *c );
    uint id = m->insertItem( p, i );
    assert( id == i );
  };
  return m;
}

void NormalModePopupObjects::doSetColor( int c )
{
  const QColor* d = color( c );
  assert( d );
  for ( Objects::const_iterator i = mobjs.begin(); i != mobjs.end(); ++i )
  {
    (*i)->setColor( *d );
  };
  mmode->clearSelection();
  mview->redrawScreen();
}

void NormalModePopupObjects::addColorPopup()
{
  QPopupMenu* colorpopup = colorMenu( this );
  connect( colorpopup, SIGNAL( activated( int ) ), this, SLOT( doSetColor( int ) ) );
  uint id = insertItem( i18n( "Set Color" ), colorpopup, colorId );
  assert( id == colorId );
}

void NormalModePopupObjects::addUsePopup()
{
  typedef std::vector<Type*> vec;
  vec t = Object::types().whoWantsArgs( mobjs );
  uint size = t.size();
  kdDebug() << k_funcinfo << "number of constructable types: "
            << size << endl;

  if ( size > 0 )
  {
    QPopupMenu* usepopup = new QPopupMenu( this, "use popup" );
    connect( usepopup, SIGNAL( activated( int ) ),
             this, SLOT( doUse( int ) ) );
    for ( uint i = 0; i < t.size(); ++i )
    {
      uint id = usepopup->insertItem( t[i]->descriptiveName(), i );
      assert( id == i );
    };
    uint id = insertItem( mobjs.size() != 1 ? i18n( "Use These Objects to Construct A")
                          : i18n( "Use This %1 to Construct A").arg(
                              mobjs[0]->vTBaseTypeName() ),
                          usepopup, useId );
    assert( id == useId );
  };
}

void NormalModePopupObjects::addHideItem()
{
  uint id = insertItem( i18n( "Hide" ), hideId );
  assert( id == hideId );
}

void NormalModePopupObjects::addMoveItem()
{
  uint id = insertItem( i18n( "Move" ), moveId );
  assert( id == moveId );
}

void NormalModePopupObjects::addDeleteItem()
{
  uint id = insertItem( i18n( "Delete" ), deleteId );
  assert( id == deleteId );
}

void NormalModePopupObjects::addVirtualItems()
{
  if ( mobjs.size() == 1 )
  {
    // we show the object-specific actions...
    mobjs[0]->addActions( *this );
  };
}

void NormalModePopupObjects::doUse( int id )
{
  typedef std::vector<Type*> vec;
  vec t = Object::types().whoWantsArgs( mobjs );
  uint size = t.size();
  kdDebug() << k_funcinfo << "number of constructable types: "
            << size << endl;

  assert( static_cast<uint>(id) < size );
  Type* typet = t[id];
  assert( typet );
  StdConstructibleType* type = typet->toStdConstructible();

  switch( type->wantArgs( mobjs ) )
  {
  case Object::NotGood:
    assert( false );
    break;
  case Object::Complete:
  {
    Object* o = type->build( mobjs );
    assert( o );
    o->calcForWidget( *mview );
    mdoc->addObject( o );
    mmode->clearSelection();
    break;
  };
  case Object::NotComplete:
  {
    KigMode* m = type->constructMode( mmode, mdoc );
    assert( m );
    StdConstructionMode* s = m->toStdConstructionMode();
    assert( s );
    assert( s->wantArgs( mobjs ) == Object::NotComplete );
    s->selectArgs( mobjs, mview );
    mdoc->setMode( s );
    break;
  };
  default:
    assert( false );
    break;
  };
}

void NormalModePopupObjects::addPopupAction( uint id, const QString& name, QPopupMenu* pm )
{
  connect( pm, SIGNAL( activated( int ) ), SLOT( doPopup( int ) ) );
  mpopupmap[pm] = id;
  insertItem( name, pm, id );
}

void NormalModePopupObjects::addNormalAction( uint id, const QString& name )
{
  uint rid = insertItem( name, id );
  assert( rid == id );
}

void NormalModePopupObjects::doPopup( int id )
{
  // sender() is a very ugly QObject member that returns the object
  // that called this slot.  Did I mention i find the entire Qt Object
  // Model ugly as hell ?  /me throws the Design Patterns book at the
  // TrollTech coders...
  // but we use it here anyway, since we need it...
  const QObject* qo = sender();
  const QPopupMenu* qp = static_cast<const QPopupMenu*>( qo );
  int popupid = mpopupmap[qp];
  assert( mobjs.size() == 1 );
  Object* o = mobjs[0];
  o->doPopupAction( popupid, id, mdoc, mview, mmode, mview->fromScreen( mview->mapFromGlobal( mplc ) ) );
}

const KigWidget& NormalModePopupObjects::widget() const
{
  return *mview;
}
