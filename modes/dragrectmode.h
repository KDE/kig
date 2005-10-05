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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.


#ifndef KIG_MODES_DRAGRECTMODE_H
#define KIG_MODES_DRAGRECTMODE_H

#include "mode.h"

#include "../misc/rect.h"

#include <qpoint.h>
#include <vector>

class ObjectHolder;

/**
 * DragRectMode is a mode that provides a rect for selecting the
 * objects inside it.  Here's an example of how to use it
 * \code
 * DragRectMode d( e->pos(), document, widget );
 * mDoc.runMode( &d );
 * Objects sel = d.ret();
 * \endcode
 */
class DragRectMode
  : public KigMode
{
  QPoint mstart;
  std::vector<ObjectHolder*> mret;
  Rect mrect;
  bool mnc;
  bool mstartselected;
  bool mcancelled;
private:
  void clicked( const QPoint& p, KigWidget& w );
  void clicked( const QMouseEvent* e, KigWidget& w );
  void released( const QPoint& p, KigWidget& w, bool nc );
  void released( QMouseEvent* e, KigWidget& w );
  void moved( const QPoint& p, KigWidget& w );
  void moved( QMouseEvent*, KigWidget& w );

  void leftClicked( QMouseEvent*, KigWidget* );
  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );
  void midClicked( QMouseEvent*, KigWidget* );
  void midMouseMoved( QMouseEvent*, KigWidget* );
  void midReleased( QMouseEvent*, KigWidget* );
  void rightClicked( QMouseEvent*, KigWidget* );
  void rightMouseMoved( QMouseEvent*, KigWidget* );
  void rightReleased( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent*, KigWidget* );

  void cancelConstruction();

  void enableActions();

public:
  DragRectMode( const QPoint& start, KigPart& d, KigWidget& w );
  DragRectMode( KigPart& d, KigWidget& w );
  ~DragRectMode();

  /**
   * this returns the selected objects..
   */
  std::vector<ObjectHolder*> ret() const;

  /**
   * this returns the selected rect..
   */
  Rect rect() const;

  /**
   * this returns false if the control or shift button were pressed
   * when the mouse button was released, and true otherwise.  This is
   * because the user expects us to not clear the selection before
   * adding the newly selected objects if (s)he pressed control or
   * shift..
   */
  bool needClear() const;

  /**
   * whether the user cancelled the rect mode..  If this returns true,
   * all the other return data above will be in undefined state, so
   * first check this function's result..
   */
  bool cancelled() const;
};

#endif
