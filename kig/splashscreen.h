/***************************************************************************
                          splashscreen.h  -  description
                             -------------------
    begin                : Sat Oct 14 2000
    copyright            : (C) 2000 by Marc Bartsch
    email                : marc.bartsch@topmail.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <qpixmap.h>
#include <qtimer.h>
#include <qdialog.h>

#include "kiconloader.h"

/**
  *@author Marc Bartsch
  */

class SplashScreen : public QDialog
{
public:
  SplashScreen();
  ~SplashScreen();
  static void showSplashScreen();
};
#endif


