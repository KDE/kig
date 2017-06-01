/**
    This file is part of Kig, a KDE program for Interactive Geometry...
    Copyright (C) 2012  David E. Narvaez <david.narvaez@computer.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef PROPERTIESACTIONSPROVIDER_H
#define PROPERTIESACTIONSPROVIDER_H

#include "popupactionprovider.h"

#include "popup.h"

class PropertiesActionsProvider
  : public PopupActionProvider
{
  // we don't really need NumberOfMenus vectors, but this is the
  // easiest way to do it, and I'm too lazy to do it properly ;)
  std::vector<int> mprops[NormalModePopupObjects::NumberOfMenus];
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree ) Q_DECL_OVERRIDE;
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m ) Q_DECL_OVERRIDE;
};

#endif // PROPERTIESACTIONSPROVIDER_H
