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

#ifndef KIG_MODES_POPUP_H
#define KIG_MODES_POPUP_H

#include <kpopupmenu.h>

#include <vector>

class KigDocument;
class KigPart;
class KigWidget;
class NormalMode;
class PopupActionProvider;
class ObjectHolder;

/**
 * This is the popup menu that appears when you click on selected
 * objects in NormalMode..  It's quite complex, since it has to fetch
 * a lot of information from various places, and dispatch it again
 * when the user selects something.
 * Update: I'm also using it for when you clicked on an empty space in
 * the document, because the difference between the two cases is not
 * that important, and this class is generic enough to handle both
 * cases..  When this is the case, mobjs is empty, some
 * PopupActionProviders are disabled, and some others enabled..
 */
class NormalModePopupObjects
  : public KPopupMenu
{
  Q_OBJECT

public:
  NormalModePopupObjects( KigPart& part, KigWidget& view,
                          NormalMode& mode, 
			  const std::vector<ObjectHolder*>& objs, const QPoint& p );
  ~NormalModePopupObjects();

  // the different "menu's", the toplevel is considered as just
  // another menu..
  enum { TransformMenu = 0, TestMenu, ConstructMenu, StartMenu, ShowMenu,
         SetColorMenu, SetSizeMenu, SetStyleMenu, ToplevelMenu,
         SetCoordinateSystemMenu, NumberOfMenus };

  // used by the PopupActionProvider's to add actions to us..
  void addAction( int menu, const QString& name, int id );
  void addAction( int menu, const QPixmap& icon, const QString& name, int id );
  void addAction( int menu, const QPixmap& pix, int id );

  // set the checked state of the n'th item in menu to checked..
  void setChecked( int menu, int n, bool checked );

  std::vector<ObjectHolder*> objects() const { return mobjs; };
  KigPart& part() { return mpart; };
  KigWidget& widget() { return mview; };
  QPoint plc() { return mplc; };

protected:
  void activateAction( int menu, int action );

private slots:
  void transformMenuSlot( int );
  void testMenuSlot( int );
  void constructMenuSlot( int );
  void startMenuSlot( int );
  void showMenuSlot( int );
  void setColorMenuSlot( int );
  void setSizeMenuSlot( int );
  void setStyleMenuSlot( int );
  void toplevelMenuSlot( int );
  void setCoordinateSystemMenuSlot( int );

protected:
  QPoint mplc;
  KigPart& mpart;
  KigWidget& mview;
  std::vector<ObjectHolder*> mobjs;
  NormalMode& mmode;

  std::vector<PopupActionProvider*> mproviders;

  QPopupMenu* mmenus[NumberOfMenus];
};

#endif
