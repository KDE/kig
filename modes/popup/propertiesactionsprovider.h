/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
