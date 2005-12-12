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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include <kaboutdata.h>
#include <klocale.h>

#include "config.h"

inline KAboutData* kigAboutData( const char* name, const char* iname )
{
  const char* version = "v" KIGVERSION;
  const char* description = I18N_NOOP( "KDE Interactive Geometry" );

  KAboutData* tmp = new KAboutData( name, iname, version,
				   description, KAboutData::License_GPL,
				   I18N_NOOP( "(C) 2002-2005, The Kig developers" ),
				   0, "http://edu.kde.org/kig" );
  tmp->addAuthor("Dominique Devriese",
                 I18N_NOOP("Original author, long time maintainer, design and lots of code."),
		  "devriese@kde.org" );

  tmp->addAuthor("Maurizio Paolini",
		 I18N_NOOP( "Did a lot of important work all around Kig, "
                            "including, but not limited to conics, cubics, "
                            "transformations and property tests support." ),
		 "paolini@dmf.unicatt.it");

  tmp->addAuthor( "Pino Toscano",
                  I18N_NOOP( "Actual maintainer, Dr. Geo import filter, point and "
                             "line styles, Italian translation, "
                             "miscellaneous stuff here and there." ),
                  "toscano.pino@tiscali.it" );

  tmp->addAuthor( "Franco Pasquarelli",
                  I18N_NOOP( "Helped a lot with the implementation of the Locus object, "
                             "there's quite some math involved in doing it right, and "
                             "Franco wrote the most difficult parts." ),
                  "pasqui@dmf.unicatt.it" );

  tmp->addCredit( "Eric Depagne",
                  I18N_NOOP( "The French translator, who also sent me some useful "
                             "feedback, like feature requests and bug reports." ),
                  "edepagne@eso.org" );

  tmp->addCredit("Marc Bartsch",
		  I18N_NOOP("Author of KGeo, where I got inspiration, "
			    "some source, and most of the artwork from." ),
		  "marc.bartsch@web.de");

  tmp->addCredit("Christophe Devriese",
                 I18N_NOOP( "Domi's brother, who he got to write the algorithm for "
                            "calculating the center of the circle with three "
                            "points given." ),
                 "oelewapperke@ulyssis.org" );

  tmp->addCredit("Christophe Prud'homme",
                 I18N_NOOP( "Sent me a patch for some bugs." ),
                 "prudhomm@mit.edu" );

  tmp->addCredit("Robert Gogolok",
                 I18N_NOOP("Gave me some good feedback on Kig, some feature "
                           "requests, cleanups and style fixes, and someone "
                           "to chat with on irc :)" ),
                 "robertgogolok@gmx.de");

  tmp->addCredit("David Vignoni",
                 I18N_NOOP("Responsible for the nice application SVG Icon." ),
                 "david80v@tin.it");

  tmp->addCredit( "Danny Allen",
                  I18N_NOOP( "Responsible for the new object action icons." ),
                  "danny@dannyallen.co.uk" );

  return tmp;
}
