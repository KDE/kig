/***************************************************************************
                          splashscreen.cpp  -  description
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

#include "splashscreen.h"

#include <kapplication.h>
#include <kconfig.h>

SplashScreen::SplashScreen() 
  : QDialog(0, 0, true, WStyle_Customize | WStyle_NoBorder) 
{
  QPixmap title( UserIcon( "splash" ) );
  resize( title.width(),title.height() );
  setBackgroundPixmap( title );
  
#ifndef NDEBUG
  QTimer::singleShot( 1500, this, SLOT( accept() ) );
#else
  QTimer::singleShot( 500, this, SLOT( accept() ) );
#endif
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::showSplashScreen()
{
  KConfig* conf = kapp->config();
  conf->setGroup("Startup");
  if (conf->readBoolEntry ("showSplashScreen", true))
  {
    SplashScreen *splash = new SplashScreen;
    splash->exec();
    delete splash;
  };
}
