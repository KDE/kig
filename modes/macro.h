// macro.h
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

#ifndef MACRO_H
#define MACRO_H

#include "mode.h"

#include "../misc/objects.h"

class MacroWizard;

class DefineMacroMode
  : public QObject, public KigMode
{
  Q_OBJECT
public:
  DefineMacroMode( KigDocument*, KigMode* previousMode );
  ~DefineMacroMode();
  void leftClicked( QMouseEvent*, KigView* );
  void leftMouseMoved( QMouseEvent*, KigView* );
  void leftReleased( QMouseEvent*, KigView* );
  void rightClicked( QMouseEvent*, KigView* );
  void rightReleased( QMouseEvent*, KigView* );
  void midReleased( QMouseEvent*, KigView* );
  void mouseMoved( QMouseEvent*, KigView* );

public slots:
  void selected( const QString& );

protected:
  void updateNexts();
  void cleanDraw();

  MacroWizard* mWizard;
  KigMode* mPrev;
  Objects* cur;

  Objects mGiven;
  Objects mFinal;
};

#endif
