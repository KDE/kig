/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KIG_PART_MODE_H
#define KIG_PART_MODE_H

//#include <qnamespace.h>

class KigDocument;
class KigPart;
class KigWidget;
class QEventLoop;
class QMouseEvent;
class StdConstructionMode;

/**
 * this is an ABC of a class containing the current "Mode" of the Kig
 * document... It tells us how to respond to a certain event.
 */
class KigMode
//  : public Qt
{
public:
  virtual ~KigMode();

  virtual StdConstructionMode* toStdConstructionMode();

  virtual void leftClicked( QMouseEvent*, KigWidget* );
  /**
   * this means: mouse moved with left mouse button down (in case that
   * wasn't clear...)
   */
  virtual void leftMouseMoved( QMouseEvent*, KigWidget* );
  virtual void leftReleased( QMouseEvent*, KigWidget* );
  virtual void midClicked( QMouseEvent*, KigWidget* );
  virtual void midMouseMoved( QMouseEvent*, KigWidget* );
  virtual void midReleased( QMouseEvent*, KigWidget* );
  virtual void rightClicked( QMouseEvent*, KigWidget* );
  virtual void rightMouseMoved( QMouseEvent*, KigWidget* );
  virtual void rightReleased( QMouseEvent*, KigWidget* );
  /**
   * mouse moved without any buttons down...
   */
  virtual void mouseMoved( QMouseEvent*, KigWidget* );

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
  virtual void browseHistory();
  virtual void selectAll();
  virtual void deselectAll();
  virtual void invertSelection();

  /**
   * Redraw the document on KigWidget \p w .  It's up to the mode to
   * refresh the screen...
   */
  virtual void redrawScreen( KigWidget* w );

  /// @internal
  void setEventLoop( QEventLoop* e );
  /// @internal
  QEventLoop* eventLoop() const;

protected:
  KigPart& mdoc;

  KigMode( KigPart& d );

private:
  QEventLoop* mevloop;
};

#endif
