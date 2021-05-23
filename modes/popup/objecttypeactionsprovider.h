/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef OBJECTTYPEACTIONSPROVIDER_H
#define OBJECTTYPEACTIONSPROVIDER_H

#include "popupactionprovider.h"

class ObjectTypeActionsProvider
  : public PopupActionProvider
{
  int mnoa;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree ) override;
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m ) override;
};

#endif // OBJECTTYPEACTIONSPROVIDER_H
