// selectionmode.h
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

#ifndef KIG_MODES_SELECTIONMODE_H
#define KIG_MODES_SELECTIONMODE_H

#include "mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

class Object;
class KigWidget;
class KigDocument;
class ArgsChecker;

class SelectionModeBase
  : public KigMode
{
  Objects mselection;
  QPoint mplc;
  Objects moco;
  bool mcswc;
  bool mret;

  void leftClicked( QMouseEvent* e, KigWidget* v );
  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent* e, KigWidget* v );
  void midClicked( QMouseEvent* e, KigWidget* v );
  void midReleased( QMouseEvent* e, KigWidget* v );
  void rightClicked( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent* e, KigWidget* v );

  void cancelConstruction();

  void enableActions();

protected:
  virtual void selectionChanged( KigWidget& ) = 0;
  virtual void dragRect( const QPoint& p, KigWidget& w );
//   virtual void movedMouse( const Objects& objectsUnderCursor, KigWidget& w );
  virtual bool wantObject( const Object& o, KigWidget& );
  virtual void rightClicked( const Objects& os, KigWidget& w );
  virtual void dragObject( const Objects& objectsClickedOn, const QPoint& pointClickedOn, KigWidget& w );

public:
  SelectionModeBase( KigDocument* );
  ~SelectionModeBase();

  void setClearSelectWithoutControl( bool b );

  bool run( KigMode* prev );

  const Objects& selection() const;

  void clearSelection( KigWidget& w );
  void selectObject( Object* o, KigWidget& w );
  void unselectObject( Object* o, KigWidget& w );
  void finish( bool ret );
};

class StandAloneSelectionMode
  : public SelectionModeBase
{
  const ArgsChecker& mchecker;
public:
  StandAloneSelectionMode( const ArgsChecker& c, KigDocument* d, bool cswc = false );
  void selectionChanged( KigWidget& w );
  virtual bool wantObject( const Object& o, KigWidget& );
};

#endif
