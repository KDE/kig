/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef POPUPACTIONPROVIDER_H
#define POPUPACTIONPROVIDER_H

#include <vector>

class KigWidget;
class KigPart;
class NormalMode;
class NormalModePopupObjects;
class ObjectHolder;

/**
 * This class is an abstract class.  Its role is to fill up the
 * NormalModePopupObjects with useful actions..
 */
class PopupActionProvider
{
public:
  virtual ~PopupActionProvider() {}
  /**
   * add all your entries to menu menu in popup popup.  Set nextfree
   * to the next free index..
   */
  virtual void fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree ) = 0;
  /**
   * try to execute the id'th action you added to menu menu in popup
   * popup ( first is 0 ).  Return true if this action does indeed
   * belong to you ( is not greater than the number of actions you
   * added ).  Else return false, and subtract the number of actions
   * you added from id.  This requires you to keep a record of how
   * much actions you added ( unless it's a fixed number, of course
   * ).
   */
  virtual bool executeAction( int menu, int& id, const std::vector<ObjectHolder*>& os,
                              NormalModePopupObjects& popup,
                              KigPart& doc, KigWidget& w, NormalMode& m ) = 0;
};

#endif // POPUPACTIONPROVIDER_H
