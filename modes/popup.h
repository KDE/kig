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

#include <map>

#include "../objects/object.h"
#include "../misc/objects.h"

using std::map;

class KigDocument;
class KigWidget;
class NormalMode;

class NormalModePopupObjects
  : public KPopupMenu
{
  Q_OBJECT

  QPoint mp;
  KigDocument* mdoc;
  KigWidget* mview;
  Objects mobjs;
  NormalMode* mmode;

  static const uint titleId = 0;
  static const uint useId = 1;
  static const uint colorId = 2;
  static const uint moveId = 3;
  static const uint deleteId = 4;
  static const uint hideId = 5;

  // see the addPopupAction() function for this...
  map<const QPopupMenu*, int> mpopupmap;

  QPopupMenu* colorMenu( QWidget* parent );
  const QColor* color( int );

  // i could do some of these as virtual actions, but they need to be
  // appliable to multiple objects at the same time...
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

  static const uint virtualActionsOffset = 6;

  // these two are the functions that objects can add objects to us
  // with, in their addActions() method...
  // here, id should be unique for all popup menu's defined by a type
  // of object..  Don't forget to take into account any id's that your
  // parent may use.  If one of these actions is selected, then
  // Object::doPopupAction() will be called with the appropriate
  // arguments ( the first one being the id you give here, and the
  // second one being the index of the action in @param qp... )
  void addPopupAction( uint id, const QString& name, QPopupMenu* qp );

  // with this function, objects may add a normal action to us.  id
  // should be unique for all actions that an object defines ( note
  // that it can be the same as id's for popup menu's given
  // above.. ).  Don't forget to consider the id's that your parent
  // uses...  If this action gets selected by the user, then
  // Object::doNormalAction will be called with the appropriate
  // arguments...
  void addNormalAction( uint id, const QString& name );

protected slots:
  void doAction( int );
  void doUse( int );
  void doSetColor( int );
  void doPopup( int );
};


#endif
