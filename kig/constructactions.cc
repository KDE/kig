// constructactions.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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

#include "constructactions.h"

#include "kig_part.h"
#include "../misc/types.h"
#include "../misc/type.h"
#include "../modes/constructing.h"
#include "../modes/normal.h"

#include <klocale.h>
#include <kiconloader.h>

#include <assert.h>

ConstructAction::ConstructAction( KigDocument* d, Type* t,
                                  const int cut )
  : KAction( i18n( t->descriptiveName() ), cut, d->actionCollection(), t->actionName() ),
    mdoc( d ),
    mtype( t )
{
  setToolTip( t->description() );
  setWhatsThis( t->description() );
  setIconSet( KGlobal::iconLoader()->loadIcon( t->iconFileName(), KIcon::User ) );
}
ConstructAction::~ConstructAction()
{
}

void ConstructAction::slotActivated()
{
  NormalMode* nm = dynamic_cast<NormalMode*>( mdoc->mode() );
  assert( nm );
  KigMode* m = mtype->constructMode( nm, mdoc );
  mdoc->setMode( m );
}

Type* ConstructAction::type()
{
  return mtype;
}
