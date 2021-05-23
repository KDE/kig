// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_ABOUTDATA_H
#define KIG_ABOUTDATA_H

#include <KAboutData>
#include <KLocalizedString>

#include <kig_version.h>

inline KAboutData kigAboutData( const char* name, const char* iname )
{
  const char* description = I18N_NOOP( "KDE Interactive Geometry" );

  KAboutData tmp( name, i18n(iname), KIG_VERSION_STRING,
				   i18n(description), KAboutLicense::GPL,
				   i18n( "(C) 2002-2005, The Kig developers" ),
				   QString(), QStringLiteral("https://edu.kde.org/kig") );

  tmp.addAuthor( i18n("David E. Narvaez"),
                 i18n("Current maintainer."),
                 QStringLiteral("david.narvaez@computer.org") );

  tmp.addAuthor(i18n("Dominique Devriese"),
                 i18n("Original author, former maintainer, design and lots of code."),
		  QStringLiteral("devriese@kde.org") );

  tmp.addAuthor(i18n("Maurizio Paolini"),
		 i18n( "Did a lot of important work all around Kig, "
                            "including, but not limited to conics, cubics, "
                            "transformations and property tests support." ),
		 QStringLiteral("paolini@dmf.unicatt.it"));

  tmp.addAuthor( i18n("Pino Toscano"),
                  i18n( "Former maintainer, Dr. Geo import filter, point and "
                             "line styles, Italian translation, "
                             "miscellaneous stuff here and there." ),
                  QStringLiteral("toscano.pino@tiscali.it") );

  tmp.addAuthor( i18n("Franco Pasquarelli"),
                  i18n( "Helped a lot with the implementation of the Locus object, "
                             "there's quite some math involved in doing it right, and "
                             "Franco wrote the most difficult parts." ),
                  QStringLiteral("pasqui@dmf.unicatt.it") );

  tmp.addAuthor( i18n("Francesca Gatti"),
                  i18n( "Responsible for the computation of the algebraic equation "
                             "of a locus." ),
                  QStringLiteral("frency.gatti@gmail.com") );

  tmp.addAuthor( i18n("Petr Gajdos"),
                  i18n( "Contributed the Bézier curves and related icons." ),
                  QStringLiteral("pgajdos@suse.cz") );

  tmp.addAuthor( i18n("Raoul Bourquin"),
                  i18n( "Contributed the 'asymptote' export filter." ),
                  QStringLiteral("raoulb@bluewin.ch") );

  tmp.addCredit( i18n("Eric Depagne"),
                  i18n( "The French translator, who also contributed some useful "
                             "feedback, like feature requests and bug reports." ),
                  QStringLiteral("edepagne@eso.org") );

  tmp.addCredit(i18n("Marc Bartsch"),
		  i18n("Author of KGeo, where I got inspiration, "
			    "some source, and most of the artwork from." ),
		  QStringLiteral("marc.bartsch@web.de"));

  tmp.addCredit(i18n("Christophe Devriese"),
                 i18n( "Domi's brother, who he got to write the algorithm for "
                            "calculating the center of the circle with three "
                            "points given." ),
                 QStringLiteral("oelewapperke@ulyssis.org") );

  tmp.addCredit(i18n("Christophe Prud'homme"),
                 i18n( "Contributed a patch for some bugs." ),
                 QStringLiteral("prudhomm@mit.edu") );

  tmp.addCredit(i18n("Robert Gogolok"),
                 i18n("Contributed some good feedback on Kig, some feature "
                           "requests, cleanups and style fixes, and someone "
                           "to chat with on irc :)" ),
                 QStringLiteral("robertgogolok@gmx.de"));

  tmp.addCredit(i18n("David Vignoni"),
                 i18n("Responsible for the nice application SVG Icon." ),
                 QStringLiteral("david80v@tin.it"));

  tmp.addCredit( i18n("Danny Allen"),
                  i18n( "Responsible for the new object action icons." ),
                  QStringLiteral("danny@dannyallen.co.uk") );

  return tmp;
}

#endif  // KIG_ABOUTDATA_H
