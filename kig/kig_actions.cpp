// kig_actions.cpp
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

#include "kig_actions.h"
#include "kig_actions.moc"

#include "kig_part.h"
#include "kig_view.h"
#include "../objects/normalpoint.h"
#include "../misc/i18n.h"
#include "../misc/coordinate_system.h"
#include "../misc/argsparser.h"
#include "../modes/selectionmode.h"

#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kdebug.h>

void AddFixedPointAction::slotActivated()
{
  bool ok;
  bool done = false;
  Coordinate c;
  while ( ! done )
  {
    QString s = KLineEditDlg::getText(
      i18n( "Fixed point" ), i18n( "Enter the coordinates for the new point.." ) +
      QString::fromUtf8("\n") + mdoc->coordinateSystem().coordinateFormatNotice(),
      QString::null, &ok, mdoc->widget() );
    if ( ! ok ) return;
    c = mdoc->coordinateSystem().toScreen( s, ok );
    if ( ok ) done = true;
    else
    {
      KMessageBox::sorry( mdoc->widget(), i18n( "The coordinate you entered was not valid.  Please try again.") );
      done = false;
    };
  };
  NormalPoint* p = NormalPoint::fixedPoint( c );
  p->calcForWidget( *mdoc->mainWidget()->realWidget() );
  mdoc->addObject( p );
};

AddFixedPointAction::AddFixedPointAction( KigDocument* doc,
                                          const QIconSet& icon,
                                          KActionCollection* coll )
  : KAction( i18n( "Point by coordinates" ), icon, 0, 0, 0, coll,
             "objects_new_point_xy" ),
    mdoc( doc )
{
    setToolTip( i18n( "Construct a point by entering its coordinates..." ) );
}

TestAction::TestAction( KigDocument* doc, const QIconSet& icon,
                        KActionCollection* parent )
  : KAction( i18n( "Test stuff !!!" ), icon, 0, 0, 0, parent,
             "test_stuff" ),
    mdoc( doc )
{
}

void TestAction::slotActivated()
{
  CheckOneArgs checker;
  StandAloneSelectionMode mode( checker, mdoc );
  mode.run( mdoc->mode() );
  Objects sel = mode.selection();
  kdDebug() << k_funcinfo << endl
            << sel.size() << endl
            << sel[0]->vBaseTypeName() << endl;
}

