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
  mDoc->enableConstructActions( false );
  mDoc->aCancelConstruction->setEnabled( false );
  mDoc->aConfigureTypes->setEnabled( false );
  mDoc->aDeleteObjects->setEnabled( false );
  mDoc->aShowHidden->setEnabled( false );
  mDoc->aNewMacro->setEnabled( false );
  mDoc->action( "edit_undo" )->setEnabled( false );
  mDoc->action( "edit_redo" )->setEnabled( false );
}

KigMode::~KigMode()
{
}

StdConstructionMode* KigMode::toStdConstructionMode()
{
  return 0;
}
