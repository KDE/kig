/*
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
*/

#ifndef KIG_PART_MODE_H
#define KIG_PART_MODE_H

#include <tqnamespace.h>

class KigDocument;
class KigPart;
class KigWidget;
class TQMouseEvent;
class StdConstructionMode;

/**
 * this is an ABC of a class containing the current "Mode" of the Kig
 * document... It tells us how to respond to a certain event.
 */
class KigMode
  : public Qt
{
public:
  virtual ~KigMode();

  virtual StdConstructionMode* toStdConstructionMode();

  virtual void leftClicked( TQMouseEvent*, KigWidget* );
  /**
   * this means: mouse moved with left mouse button down (in case that
   * wasn't clear...)
   */
  virtual void leftMouseMoved( TQMouseEvent*, KigWidget* );
  virtual void leftReleased( TQMouseEvent*, KigWidget* );
  virtual void midClicked( TQMouseEvent*, KigWidget* );
  virtual void midMouseMoved( TQMouseEvent*, KigWidget* );
  virtual void midReleased( TQMouseEvent*, KigWidget* );
  virtual void rightClicked( TQMouseEvent*, KigWidget* );
  virtual void rightMouseMoved( TQMouseEvent*, KigWidget* );
  virtual void rightReleased( TQMouseEvent*, KigWidget* );
  /**
   * mouse moved without any buttons down...
   */
  virtual void mouseMoved( TQMouseEvent*, KigWidget* );

  /**
   * actions: we enable the actions we want when our mode is made
   * active.  These actions are members of KigDocument, and call slots
   * on KigDocument.  These slots all call the correspondent mode()
   * member.  Modes reimplement the ones they need, and turn on the
   * actions they support in enableActions().
   */
  virtual void enableActions();

  virtual void cancelConstruction();
  virtual void deleteObjects();
  virtual void showHidden();
  virtual void newMacro();
  virtual void editTypes();
  virtual void selectAll();
  virtual void deselectAll();
  virtual void invertSelection();

  /**
   * Redraw the document on KigWidget \p w .  It's up to the mode to
   * refresh the screen...
   */
  virtual void redrawScreen( KigWidget* w );
protected:
  KigPart& mdoc;

  KigMode( KigPart& d );
};

#endif
