#include "filter.h"

#include "kgeo.h"
#include "cabri.h"

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
  mFilters.push_back( new KigFilterCabri );
}
