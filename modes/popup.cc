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
#include "normal.h"
#include "moving.h"

#include <qcursor.h>

NormalModePopupObjects::NormalModePopupObjects( KigDocument* doc,
                                                KigView* view,
                                                NormalMode* mode,
                                                const Objects& objs )
  : mdoc( doc ), mview( view ), mobjs( objs ), mmode( mode ),
    mcolorpopup( 0 )
{
  assert ( ! objs.empty() );
  bool single = objs.size() == 1;
  connect( this, SIGNAL( activated( int ) ), this, SLOT( doAction( int ) ) );
  // title..
  insertTitle( single ? objs[0]->vTBaseTypeName()
               : i18n( "%1 Objects" ).arg( objs.size() ), 0 );

  // set the color
  mcolorpopup = colorMenu( this );
  connect( mcolorpopup, SIGNAL( activated( int ) ),
           this, SLOT( setColor( int ) ) );
  int id = insertItem( i18n( "Set Color..." ), mcolorpopup, 1 );
  assert( id == 1 );

  // hide action..
  id = insertItem( i18n( "Hide..." ), 2 );
  assert( id == 2 );

  // delete action..
  id = insertItem( i18n( "Delete..." ), 3 );
  assert( id == 3 );

  // move action...
  id = insertItem( i18n( "Move..." ), 4 );
  assert( id == 4 );
}

void NormalModePopupObjects::doAction( int i )
{
  kdDebug() << k_funcinfo << " i == " << i << endl;
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
    QRect r = this->frameRect();
    QPoint p = r.topLeft();
    p = this->mapToParent( p );
    Coordinate c = mview->fromScreen( p );
    MovingMode* m = new MovingMode( mobjs, c, mmode, mview, mdoc );
    mdoc->setMode( m );
    // in this case, we return, cause we don't want objects to be
    // unselected... ( or maybe we do ? )
    return;
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
  for ( Objects::const_iterator i = mobjs.begin(); i != mobjs.end(); ++i )
  {
    (*i)->setColor( *d );
  };
  mmode->clearSelection();
  mview->redrawScreen();
}

