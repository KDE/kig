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

#ifndef SCRIPTACTIONSPROVIDER_H
#define SCRIPTACTIONSPROVIDER_H

#include <config-kig.h>

#ifdef KIG_ENABLE_PYTHON_SCRIPTING

#include "popupactionprovider.h"

#include "../scripting/script-common.h"
#include "../scripting/script_mode.h"
#include "../scripting/python_type.h"

class ScriptActionsProvider
  : public PopupActionProvider
{
  int mns;
public:
  void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree );
  bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                      NormalModePopupObjects& popup,
                      KigPart& doc, KigWidget& w, NormalMode& m );
};

#endif

#endif // SCRIPTACTIONSPROVIDER_H
