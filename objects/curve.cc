// curve.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "curve.h"

#include "../modes/popup.h"
#include "../modes/normal.h"
#include "../misc/i18n.h"
#include "../kig/kig_view.h"

#include <kiconloader.h>
#include <qpopupmenu.h>

Curve::Curve()
  : mWidth( 1 )
{
};

bool Curve::isa( int type ) const
{
  return type == CurveT ? true : Parent::isa( type );
}

void Curve::addActions( NormalModePopupObjects& p )
{
  bool ok = false;
  if ( this->toAbstractLine() ) ok = true;
  if ( this->toConic() ) ok = true;
//  if ( this->toCircle() ) ok = false;
  if ( ! ok ) return;

  QPopupMenu* pop = new QPopupMenu( &p, "curve virtual popup" );
  uint id;
  id = pop->insertItem( UserIcon( "line1"), 1 );
  assert ( id == 1 );
  id = pop->insertItem( UserIcon( "line2"), 2 );
  assert ( id == 2 );
  id = pop->insertItem( UserIcon( "line3"), 3 );
  assert ( id == 3 );
  id = pop->insertItem( UserIcon( "line4"), 4 );
  assert ( id == 4 );
  id = pop->insertItem( UserIcon( "line5"), 5 );
  assert ( id == 5 );
// CHECK...
  p.addPopupAction( 10, i18n( "Set Width" ), pop );
}

void Curve::doPopupAction( int popupid, int actionid, KigDocument*, KigWidget* w, NormalMode* m, const Coordinate& )
{
  if ( popupid == 10 )
  {
    mWidth = actionid;
    m->clearSelection();
    w->redrawScreen();
  };
}
