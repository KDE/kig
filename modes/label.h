// label.h
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

#ifndef KIG_MODE_LABEL_H
#define KIG_MODE_LABEL_H

#include "mode.h"

#include "../misc/coordinate.h"

#include <vector>

class TextLabelWizard;
class NormalMode;
class Object;
class PropertyObject;

class TextLabelConstructionMode
  : public KigMode
{
  // point last clicked..
  QPoint mplc;

  Coordinate mcoord;
  QString mtext;
  typedef std::vector<PropertyObject*> argvect;
  argvect margs;

  // if we're ReallySelectingArgs, then this var points to the arg
  // we're currently selecting...
  int mwaaws;

  // last percent count...
  uint mlpc;

  TextLabelWizard* mwiz;

  // What Are We Doing...
  // the diff between SelectingArgs and ReallySelectingArgs is that
  // the latter means the user is selecting an arg in the kig window,
  // whereas the first only means that he's looking at the second
  // page of the wizard...
  enum { SelectingLocation, RequestingText, SelectingArgs, ReallySelectingArgs } mwawd;

  void updateWiz();
  void updateLinksLabel();

public:
  TextLabelConstructionMode( KigDocument& d );
  ~TextLabelConstructionMode();
  void leftClicked( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );

  void mouseMoved( QMouseEvent*, KigWidget* );

  void enableActions();

  void cancelConstruction();

  void killMode();

public:
  // below is the interface towards TextLabelWizard...
  void cancelPressed();
  void finishPressed();
  void enterTextPageEntered();
  void selectArgumentsPageEntered();
  void labelTextChanged();
  void linkClicked( int );
};

#endif
