/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#ifndef KIG_PART_MODE_H
#define KIG_PART_MODE_H

#include <qnamespace.h>

class KigWidget;
class KigDocument;
class QMouseEvent;
class StdConstructionMode;

/**
 * this is an ABC of a class containing the current "Mode" of the kig
 * document... It tells us how to respond to a certain event.
 */
class KigMode
  : public Qt
{
public:
  virtual ~KigMode() = 0;

  virtual StdConstructionMode* toStdConstructionMode();
  virtual void leftClicked( QMouseEvent*, KigWidget* ) {};
  // this means: mouse moved with left mouse button down (in case that
  // wasn't clear...)
  virtual void leftMouseMoved( QMouseEvent*, KigWidget* ) {};
  virtual void leftReleased( QMouseEvent*, KigWidget* ) {};
  virtual void midClicked( QMouseEvent*, KigWidget* ) {};
  virtual void midMouseMoved( QMouseEvent*, KigWidget* ) {};
  virtual void midReleased( QMouseEvent*, KigWidget* ) {};
  virtual void rightClicked( QMouseEvent*, KigWidget* ) {};
  virtual void rightMouseMoved( QMouseEvent*, KigWidget* ) {};
  virtual void rightReleased( QMouseEvent*, KigWidget* ) {};
  // mouse moved without any buttons down...
  virtual void mouseMoved( QMouseEvent*, KigWidget* ) {};

  // actions: we enable the actions we want when our mode is made
  // active.  These actions are members of KigDocument, and call slots
  // on KigDocument.  These slots all call the correspondent mode()
  // member.  Modes reimplement the ones they need, and turn on the
  // actions they support in enableActions().
  virtual void enableActions();

  virtual void cancelConstruction() {};
  virtual void deleteObjects() {};
  virtual void showHidden() {};
  virtual void newMacro() {};
  virtual void editTypes() {};
  virtual void startKiosk() {};

  // two special functions: because kig has undo/redo-stuff via
  // KCommands and KCommandHistory, the mode doesn't entirely control
  // when an object is added or removed.  These slots are called
  // whenever KigAddObjectsCommand or KigRemoveObjectsCommand add or
  // remove objects from the document.  KigDocument::addObject
  // internally works with Commands, so if you called that, one of
  // these funcs are going to get called next...  It's up to the mode
  // to refresh the screen...
  // Note: a mode doesn't need these functions unless it calls
  // mDoc->history->updateActions() in enableActions()...
  virtual void objectsAdded() {};
  virtual void objectsRemoved() {};
protected:
  KigMode( KigDocument* d ) : mDoc( d ) {};
  // FIXME: figure out what to do when the KigDocument deletes its
  // KigMode* member.. we currently have memory leaks...
  KigDocument* mDoc;
};

#endif
