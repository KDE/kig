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

#ifndef KIG_POPUP_H
#define KIG_POPUP_H

#include <kpopupmenu.h>

#include "../objects/object.h"
#include "../misc/objects.h"

class KigDocument;
class KigWidget;
class NormalMode;

class NormalModePopupObjects
  : public KPopupMenu
{
  Q_OBJECT

  static const uint titleId = 0;
  static const uint useId = 1;
  static const uint colorId = 2;
  static const uint moveId = 3;
  static const uint deleteId = 4;
  static const uint hideId = 5;
  static const uint restOffset = 6;

  QPoint mp;
  KigDocument* mdoc;
  KigWidget* mview;
  Objects mobjs;
  NormalMode* mmode;

  QPopupMenu* colorMenu( QWidget* parent );
  const QColor* color( int );

  void addColorPopup();
  void addUsePopup();
  void addHideItem();
  void addMoveItem();
  void addDeleteItem();
  void addVirtualItems();       // this adds the objects own actions (
                                // see Object::objectActions() )...

public:
  NormalModePopupObjects( KigDocument* doc, KigWidget* view,
                          NormalMode* mode, const Objects& objs );
  ~NormalModePopupObjects() {};

protected slots:
  void doAction( int );
  void doUse( int );
  void setColor( int );
};


#endif
