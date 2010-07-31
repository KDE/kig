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

#include "mode.h"

#include "../kig/kig_part.h"

#include <kaction.h>

void KigMode::enableActions()
{
  mdoc.enableConstructActions( false );
  mdoc.aSelectAll->setEnabled( false );
  mdoc.aDeselectAll->setEnabled( false );
  mdoc.aInvertSelection->setEnabled( false );
  mdoc.aCancelConstruction->setEnabled( false );
  mdoc.aConfigureTypes->setEnabled( false );
  mdoc.aDeleteObjects->setEnabled( false );
  mdoc.aShowHidden->setEnabled( false );
  mdoc.aNewMacro->setEnabled( false );
  mdoc.action( "edit_undo" )->setEnabled( false );
  mdoc.action( "edit_redo" )->setEnabled( false );
}

KigMode::~KigMode()
{
}

KigMode::KigMode( KigPart& d )
  : mdoc( d )
{
}

void KigMode::leftClicked( TQMouseEvent*, KigWidget* )
{
}

void KigMode::leftMouseMoved( TQMouseEvent*, KigWidget* )
{
}

void KigMode::leftReleased( TQMouseEvent*, KigWidget* )
{
  /* insist disabling the undo button to avoid crashes */
  mdoc.action( "edit_undo" )->setEnabled( false );
  mdoc.action( "edit_redo" )->setEnabled( false );
}

void KigMode::midClicked( TQMouseEvent*, KigWidget* )
{
}

void KigMode::midMouseMoved( TQMouseEvent*, KigWidget* )
{
}

void KigMode::midReleased( TQMouseEvent*, KigWidget* )
{
}

void KigMode::rightClicked( TQMouseEvent*, KigWidget* )
{
}

void KigMode::rightMouseMoved( TQMouseEvent*, KigWidget* )
{
}

void KigMode::rightReleased( TQMouseEvent*, KigWidget* )
{
}

void KigMode::mouseMoved( TQMouseEvent*, KigWidget* )
{
}

void KigMode::cancelConstruction()
{
}

void KigMode::deleteObjects()
{
}

void KigMode::showHidden()
{
}

void KigMode::newMacro()
{
}

void KigMode::editTypes()
{
}

void KigMode::redrawScreen( KigWidget* )
{
}

StdConstructionMode* KigMode::toStdConstructionMode()
{
  return 0;
}

void KigMode::selectAll()
{
}

void KigMode::deselectAll()
{
}

void KigMode::invertSelection()
{
}
