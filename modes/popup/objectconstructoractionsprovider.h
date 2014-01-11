/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  David E. Narvaez <david@piensalibre.net>

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


#ifndef OBJECTCONSTRUCTORACTIONSPROVIDER_H
#define OBJECTCONSTRUCTORACTIONSPROVIDER_H

#include "popupactionprovider.h"

#include "popup.h"

class ObjectConstructor;

class ObjectConstructorActionsProvider
  : public PopupActionProvider
{
  std::vector<ObjectConstructor*> mctors[NormalModePopupObjects::NumberOfMenus];
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

#endif // OBJECTCONSTRUCTORACTIONSPROVIDER_H