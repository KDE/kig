/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "filter.h"

#include "kgeo-filter.h"
#include "cabri-filter.h"
#include "native-filter.h"
#include "kseg-filter.h"
#include "drgeo-filter.h"

#include <kmessagebox.h>
#include <klocale.h>

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
                            "its permissions" ).arg( file ) );
}

void KigFilter::parseError( const QString& file, const QString& explanation ) const
{
  const QString text =
    i18n( "An error was encountered while parsing the file \"%1\".  It "
          "cannot be opened." ).arg( file );
  const QString title = i18n( "Parse Error" );

  if ( explanation.isNull() )
    KMessageBox::sorry( 0, text, title );
  else
    KMessageBox::detailedSorry( 0, text, explanation, title );
}

void KigFilter::notSupported( const QString& file, const QString& explanation ) const
{
  KMessageBox::detailedSorry( 0,
                              i18n( "Kig cannot open the file \"%1\"." ).arg( file ),
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

/*
bool KigFilters::save( const KigDocument& data, QTextStream& stream )
{
  return KigFilterNative::instance()->save( data, stream );
}
*/
