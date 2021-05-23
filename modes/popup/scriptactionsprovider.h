/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SCRIPTACTIONSPROVIDER_H
#define SCRIPTACTIONSPROVIDER_H

#include <config-kig.h>

#ifdef KIG_ENABLE_PYTHON_SCRIPTING

#include "popupactionprovider.h"

#include "../../scripting/script-common.h"
#include "../../scripting/script_mode.h"
#include "../../scripting/python_type.h"

class ScriptActionsProvider
  : public PopupActionProvider
{
  int mns;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree ) override;
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m ) override;
};

#endif

#endif // SCRIPTACTIONSPROVIDER_H
