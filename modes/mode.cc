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

#include "mode.h"

#include "../kig/kig_part.h"

#include <kaction.h>

void KigMode::enableActions()
{
  mdoc.enableConstructActions( false );
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

KigMode::KigMode( KigDocument& d )
  : mdoc( d )
{
}

void KigMode::leftClicked( QMouseEvent*, KigWidget* )
{
}

void KigMode::leftMouseMoved( QMouseEvent*, KigWidget* )
{
}

void KigMode::leftReleased( QMouseEvent*, KigWidget* )
{
}

void KigMode::midClicked( QMouseEvent*, KigWidget* )
{
}

void KigMode::midMouseMoved( QMouseEvent*, KigWidget* )
{
}

void KigMode::midReleased( QMouseEvent*, KigWidget* )
{
}

void KigMode::rightClicked( QMouseEvent*, KigWidget* )
{
}

void KigMode::rightMouseMoved( QMouseEvent*, KigWidget* )
{
}

void KigMode::rightReleased( QMouseEvent*, KigWidget* )
{
}

void KigMode::mouseMoved( QMouseEvent*, KigWidget* )
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

void KigMode::startKiosk()
{
}

void KigMode::redrawScreen()
{
}

StdConstructionMode* KigMode::toStdConstructionMode()
{
  return 0;
}
