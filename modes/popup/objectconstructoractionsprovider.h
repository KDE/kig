/*
    <one line to give the program's name and a brief idea of what it does.>
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david@piensalibre.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "popupactionprovider.h"

#include "popup.h"

class ObjectConstructor;

class ObjectConstructorActionsProvider : public PopupActionProvider
{
    std::vector<ObjectConstructor *> mctors[NormalModePopupObjects::NumberOfMenus];

public:
    void fillUpMenu(NormalModePopupObjects &popup, int menu, int &nextfree) override;
    bool
    executeAction(int menu, int &id, const std::vector<ObjectHolder *> &os, NormalModePopupObjects &popup, KigPart &doc, KigWidget &w, NormalMode &m) override;
};
