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

#include "popup.h"
#include "popup.moc"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/i18n.h"
#include "../modes/mode.h"

#include <qcursor.h>

// KigObjectsPopup::KigObjectsPopup( KigDocument* d, KigView* v, const Objects& os )
//   : QPopupMenu( v, "Object Popup Menu" ),
//     mDoc( d ),
//     mView( v ),
//     mObjs( os ),
//     mValid(true),
//     mColorPopup( 0 )
// {
// /*    if( os.count() == 0 )*/
// /*     {*/
// /*       mValid = false;*/
// /*       return;*/
// /*     };*/
// /*   mValid = true;*/
// /*   if( os.count() == 1 )*/
// /*     {*/
// /*       // these are only useful if there is only one object...*/
// /*       Object* o = os.getFirst();*/
// /*       if( mDoc->canSelectObject( o ) && !o->getSelected() )*/
// /* 	insertItem( i18n( "Select this %1" ).arg( o->vTBaseTypeName() ),*/
// /* 		    this, SLOT( select() ) );*/
// /*       if( mDoc->canUnselect() && o->getSelected() )*/
// /* 	insertItem( i18n( "Unselect this %1" ).arg( o->vTBaseTypeName() ),*/
// /* 		    this, SLOT( unselect() ) );*/
// /*       if( mDoc->canMoveObjects() )*/
// /* 	insertItem( i18n( "Start moving this %1" ).arg( o->vTBaseTypeName() ),*/
// /* 		    this, SLOT( startMoving() ) );*/
// /*       if( mDoc->canHideObjects() )*/
// /* 	insertItem( i18n( "Hide this %1" ).arg( o->vTBaseTypeName() ),*/
// /* 		    this, SLOT( hideObjects() ) );*/
// /*     }*/
// /*   else*/
// /*     {*/
// /*       // these are only useful if there are more than one objects...*/
// /*       bool allUnselected = true;*/
// /*       bool allSelected = true;*/
// /*       for (Object* i = mObjs.first(); i; i = mObjs.next())*/
// /* 	{*/
// /* 	  if( i->getSelected() ) allUnselected = false;*/
// /* 	  else allSelected = false;*/
// /* 	};*/
// /*       if( mDoc->canUnselect() && !allUnselected )*/
// /* 	insertItem( i18n( "Unselect %1 objects" ).arg( mObjs.count() ),*/
// /* 		    this, SLOT( unselect() ) );*/
// /*       if( mDoc->canHideObjects() )*/
// /* 	insertItem( i18n( "Hide %1 objects" ).arg( mObjs.count() ),*/
// /* 		    this, SLOT( hideObjects() ) );*/
// /*     };*/
// }

// KigObjectsPopup::~KigObjectsPopup()
// {

// }

// void KigObjectsPopup::selectObjects()
// {
// /*   mDoc->clearSelection();*/
// /*   mDoc->selectObjects( mObjs );*/
// }

// void KigObjectsPopup::startMoving()
// {
// /*   Object* o = mObjs.first();*/
// /*   if( !o->getSelected() )*/
// /*     {*/
// /*       mDoc->clearSelection();*/
// /*       mDoc->selectObject( o );*/
// /*     };*/
// /*   mView->startMovingSos( mView->mapFromGlobal( mStart ) );*/
// }

// void KigObjectsPopup::hideObjects()
// {
// /*   mDoc->hideObjects( mObjs );*/
// }

// void KigObjectsPopup::unselectObjects()
// {
// /*   mDoc->unselect( mObjs );*/
// }

// int KigObjectsPopup::exec( const QPoint& p )
// {
//   kdDebug() << k_funcinfo << endl;
//   mStart = p;
//   return QPopupMenu::exec( p );
// }

// QPopupMenu* KigObjectsPopup::colorMenu( QWidget* parent )
// {
//   QPopupMenu* m = 0;
//   m = new QPopupMenu( parent, "color popup menu" );
//   const QColor* c = 0;
//   QPixmap p( 50, 20 );
//   for( int i = 0; ( c = color( i ) ); ++i )
//     {
//       p.fill( *c );
//       m->insertItem( p );
//     };
//   return m;
// }

// void KigObjectsPopup::setColor( int c )
// {
//   const QColor* d = color( mColorPopup->indexOf(c) );
//   assert( d );
// /*   mDoc->setColor( mObjs, *d );*/
// }

// const QColor* KigObjectsPopup::color( int i )
// {
//   static const QColor* colors[] =
//     { &Qt::blue,
//       &Qt::black,
//       &Qt::gray,
//       &Qt::lightGray,
//       &Qt::green,
//       &Qt::cyan,
//       &Qt::yellow,
//       &Qt::darkRed,
//       0
//     };
//   if( i < 0 || i > 8 ) return 0;
//   return colors[i];
// }

NormalModePopupObjects::NormalModePopupObjects( KigDocument* doc,
                                                KigView* view,
                                                NormalMode* mode,
                                                const Objects& objs )
  : mdoc( doc ), mview( view ), mobjs( objs ), mmode( mode ),
    mcolorpopup( 0 )
{
  assert ( ! objs.empty() );
  bool single = objs.size() == 1;
  // title..
  insertTitle( single ? objs[0]->vTBaseTypeName()
               : i18n( "%1 Objects" ).arg( objs.size() ), 0 );

  // set the color
  mcolorpopup = colorMenu( this );
  connect( mcolorpopup, SIGNAL( activated( int ) ),
           this, SLOT( setColor( int ) ) );
  insertItem( i18n( "Set Color..." ), mcolorpopup, 1 );

  // hide action..
  insertItem( i18n( "Hide..." ), (QObject*) 0, 0, 2 );

  // delete action..
  insertItem( i18n( "Delete..." ), (QObject*) 0, 0, 3 );

  // move action...
  insertItem( i18n( "Move..." ), (QObject*) 0, 0, 4 );
}

void NormalModePopupObjects::activated( int i )
{
  assert( 1 < i && i < 5 );
  switch( i )
  {
  case 2:
    // hide action..
    std::for_each( mobjs.begin(), mobjs.end(),
                   std::bind2nd( std::mem_fun( &Object::setShown ), false ) );
    mdoc->mode()->objectsRemoved();
    break;
  case 3:
    // delete action..
    mdoc->delObjects( mobjs );
    break;
  case 4:
    // move action..
//    mdoc->setMode( MovingMode... )
    break;
  };
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

void NormalModePopupObjects::setColor( int c )
{
  const QColor* d = color( mcolorpopup->indexOf(c) );
  assert( d );
// TODO..
}

