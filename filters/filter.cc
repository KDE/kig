/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filter.h"

#include "kgeo-filter.h"
#include "cabri-filter.h"
#include "native-filter.h"
#include "kseg-filter.h"
#include "drgeo-filter.h"
#ifdef WITH_GEOGEBRA
#include "geogebra-filter.h"
#endif //WITH_GEOGEBRA

#include <KLocalizedString>
#include <KMessageBox>

KigFilters* KigFilters::sThis;

KigFilter* KigFilters::find(const QString& mime)
{
  for (vect::iterator i = mFilters.begin(); i != mFilters.end(); ++i)
    {
      if ((*i)->supportMime(mime)) return *i;
    };
  return 0;
}

KigFilters::KigFilters()
{
  mFilters.push_back( KigFilterKGeo::instance() );
  mFilters.push_back( KigFilterKSeg::instance() );
  mFilters.push_back( KigFilterCabri::instance() );
  mFilters.push_back( KigFilterNative::instance() );
  mFilters.push_back( KigFilterDrgeo::instance() );
#ifdef WITH_GEOGEBRA
  mFilters.push_back( KigFilterGeogebra::instance() );
#endif //WITH_GEOGEBRA
}

KigFilters* KigFilters::instance()
{
  return sThis ? sThis : ( sThis = new KigFilters() );
}

KigFilter::KigFilter()
{
}

KigFilter::~KigFilter()
{
}

bool KigFilter::supportMime( const QString& )
{
  return false;
}

void KigFilter::fileNotFound( const QString& file ) const
{
  KMessageBox::sorry( 0,
                      i18n( "The file \"%1\" could not be opened.  "
                            "This probably means that it does not "
                            "exist, or that it cannot be opened due to "
                            "its permissions", file ) );
}

void KigFilter::parseError( const QString& explanation ) const
{
  const QString text =
    i18n( "An error was encountered while parsing this file.  It "
          "cannot be opened." );
  const QString title = i18n( "Parse Error" );

  if ( explanation.isEmpty() )
    KMessageBox::sorry( 0, text, title );
  else
    KMessageBox::detailedSorry( 0, text, explanation, title );
}

void KigFilter::notSupported( const QString& explanation ) const
{
  KMessageBox::detailedSorry( 0,
                              i18n( "Kig cannot open this file." ),
                              explanation, i18n( "Not Supported" ) );
}

void KigFilter::warning( const QString& explanation ) const
{
  KMessageBox::information( 0, explanation );
}

bool KigFilters::save( const KigDocument& data, const QString& tofile )
{
  return KigFilterNative::instance()->save( data, tofile );
}
