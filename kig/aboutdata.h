// aboutdata.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include <kaboutdata.h>
#include <klocale.h>

inline KAboutData* kigAboutData( const char* name, const char* iname )
{
  const char* version = "v0.3";
  const char* description = I18N_NOOP( "KDE Interactive Geometry" );

  KAboutData* tmp = new KAboutData( name, iname, version,
				   description, KAboutData::License_GPL,
				   "(C) 2002, Dominique Devriese");
  tmp->addAuthor("Dominique Devriese", I18N_NOOP("Coding"),
		  "devriese@kde.org" );

  tmp->addCredit("Marc Bartsch",
		  I18N_NOOP("Author of KGeo, where i got inspiration, "
			    "some source, and most of the artwork from"),
		  "marc.bartsch@web.de");

  tmp->addCredit("Ilya Baran",
		  I18N_NOOP("Author of KSeg, another program that has been a "
			    "source of inspiration for Kig"),
		  "ibaran@mit.edu");

  tmp->addCredit("Maurizio Paolini",
		 I18N_NOOP( "Wrote the radical line and the circle-line "
                            "intersection as a patch for kgeo, which i "
                            "ported to kig. Gave me some cool ideas on the "
                            "object system, and did some really _great_ work "
                            "on Kig's Conic support." ),
		 "paolini@dmf.bs.unicatt.it");

  tmp->addCredit("Christophe Devriese",
                 I18N_NOOP( "My brother, I got him to write the algorithm for "
                            "calculating the center of the circle with three "
                            "points given..." ),
                 "oelewapperke@ulyssis.org" );

  tmp->addCredit("Christophe Prud'homme",
                 I18N_NOOP( "Sent me a patch for some bugs..." ),
                 "prudhomm@mit.edu" );

  tmp->addCredit("Cabri coders",
		  I18N_NOOP("Cabri is a commercial program like Kig, and "
			    "gave me something to compete against :)"),
		  "www-cabri.imag.fr");

  tmp->addCredit("Robert Gogolok",
                 I18N_NOOP("Gave me some good feedback on Kig, some feature "
                           "requests, cleanups and style fixes, and someone "
                           "to chat with on irc :)" ),
                 "robertgogolok@gmx.de");

  return tmp;
};
