/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mode.h"

#include "../kig/kig_part.h"

#include <QAction>

void KigMode::enableActions()
{
  mdoc.enableConstructActions( false );
  mdoc.aSelectAll->setEnabled( false );
  mdoc.aDeselectAll->setEnabled( false );
  mdoc.aInvertSelection->setEnabled( false );
  mdoc.aCancelConstruction->setEnabled( false );
  mdoc.aConfigureTypes->setEnabled( false );
  mdoc.aBrowseHistory->setEnabled( false );
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
  : mdoc( d ), mevloop( 0 )
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
  /* insist disabling the undo button to avoid crashes */
  mdoc.action( "edit_undo" )->setEnabled( false );
  mdoc.action( "edit_redo" )->setEnabled( false );
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

void KigMode::browseHistory()
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

void KigMode::setEventLoop( QEventLoop* e )
{
  mevloop = e;
}

QEventLoop* KigMode::eventLoop() const
{
  return mevloop;
}

