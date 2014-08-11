/**
    This file is part of Kig, a KDE program for Interactive Geometry...
    Copyright (C) 2012  David E. Narvaez <david.narvaez@computer.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "objectchooserpopup.h"

#include "../../objects/object_holder.h"
#include "../../objects/point_imp.h"
#include "../../objects/polygon_imp.h"

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
    else if ( (*i)->imp()->inherits( FilledPolygonImp::stype() ) ) numpolygons++;
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

  return id;
}

ObjectChooserPopup::ObjectChooserPopup( const QPoint& p, KigWidget& view,
                                        const std::vector<ObjectHolder*>& objs )
  : QMenu(), mplc( p ), mview( view ), mobjs( objs ), mselected( -1 )
{
  addSection( i18np( "%1 Object", "%1 Objects", mobjs.size() ) );
  QAction* newaction = 0;
  for ( uint i = 0; i < mobjs.size(); i++ )
  {
    newaction = addAction(
                !mobjs[i]->name().isEmpty()
                ? QString::fromLatin1( "%1 %2" ).arg( mobjs[i]->imp()->type()->translatedName() ).arg( mobjs[i]->name() )
                : mobjs[i]->imp()->type()->translatedName() );
    newaction->setData( QVariant::fromValue( i ) );
  }

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( actionActivatedSlot( QAction* ) ) );
}

ObjectChooserPopup::~ObjectChooserPopup()
{
}

void ObjectChooserPopup::actionActivatedSlot( QAction* act )
{
  mselected = act->data().toInt();
}
