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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#include "filter.h"

#include "kgeo-filter.h"
#include "cabri-filter.h"
#include "native-filter.h"
#include "kseg-filter.h"

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
  mFilters.push_back( new KigFilterKGeo );
  mFilters.push_back( new KigFilterKSeg );
//   mFilters.push_back( new KigFilterCabri );
  mFilters.push_back( new KigFilterNative );
}

KigFilters* KigFilters::instance()
{
  return sThis ? sThis : ( sThis = new KigFilters() );
}

KigFilter::Result KigFilter::save( const KigDocument&, const QString& )
{
  // most filters don't have saving...
  return NotSupported;
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
