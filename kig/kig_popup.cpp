/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
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

#include "kig_popup.h"

#include "kig_part.h"
#include "kig_view.h"

#include <qcursor.h>

KigObjectsPopup::KigObjectsPopup( KigDocument* d, KigView* v, const Objects& os )
  : QPopupMenu( d->widget(), "Object Popup Menu" ),
    mDoc( d ),
    mView( v ),
    mObjs( os ),
    mValid(true),
    mColorPopup( 0 )
{
  if( os.count() == 0 )
    {
      mValid = false;
      return;
    };
  mValid = true;
  if( os.count() == 1 )
    {
      // these are only useful if there is only one object...
      Object* o = os.getFirst();
      if( mDoc->canSelectObject( o ) && !o->getSelected() )
	insertItem( i18n( "Select this %1" ).arg( o->vTBaseTypeName() ),
		    this, SLOT( select() ) );
      if( mDoc->canUnselect() && o->getSelected() )
	insertItem( i18n( "Unselect this %1" ).arg( o->vTBaseTypeName() ),
		    this, SLOT( unselect() ) );
      if( mDoc->canMoveObjects() )
	insertItem( i18n( "Start moving this %1" ).arg( o->vTBaseTypeName() ),
		    this, SLOT( startMoving() ) );
    }
  else
    {
      // these are only useful if there are more than one objects...
      bool allUnselected = true;
      bool allSelected = true;
      for (Object* i = mObjs.first(); i; i = mObjs.next())
	{
	  if( i->getSelected() ) allUnselected = false;
	  else allSelected = false;
	};
      if( mDoc->canUnselect() && !allUnselected )
	insertItem( i18n( "Unselect %1 objects" ).arg( mObjs.count() ),
		    this, SLOT( unselect() ) );
    };
  mColorPopup = colorMenu( this );
  connect( mColorPopup, SIGNAL( activated( int ) ), this, SLOT( setColor( int ) ) );
  insertItem( i18n( "Set Color..." ), mColorPopup );
}

KigObjectsPopup::~KigObjectsPopup()
{
    
}

void KigObjectsPopup::select()
{
  mDoc->clearSelection();
  mDoc->selectObjects( mObjs );
}

void KigObjectsPopup::startMoving()
{
  Object* o = mObjs.first();
  if( !o->getSelected() )
    {
      mDoc->clearSelection();
      mDoc->selectObject( o );
    };
  mView->startMovingSos( mView->mapFromGlobal( mStart ) );
}

void KigObjectsPopup::unselect()
{
  mDoc->unselect( mObjs );
}

int KigObjectsPopup::exec( const QPoint& p )
{
  kdDebug() << k_funcinfo << endl;
  mStart = p;
  return QPopupMenu::exec( p );
}

QPopupMenu* KigObjectsPopup::colorMenu( QWidget* parent )
{
  QPopupMenu* m = 0;
  m = new QPopupMenu( parent, "color popup menu" );
  const QColor* c = 0;
  QPixmap p( 50, 20 );
  for( int i = 0; ( c = color( i ) ); ++i )
    {
      p.fill( *c );
      m->insertItem( p );
    };
  return m;
}

void KigObjectsPopup::setColor( int c )
{
  const QColor* d = color( mColorPopup->indexOf(c) );
  assert( d );
  mDoc->setColor( mObjs, *d );
}

const QColor* KigObjectsPopup::color( int i )
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
