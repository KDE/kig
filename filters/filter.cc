#include "filter.h"

#include "kgeo.h"
#include "cabri.h"

KigFilters::vect KigFilters::m_filters;
KigFilters* KigFilters::sThis;

KigFilter* KigFilters::find(const QString& mime)
{
  for (vect::iterator i = m_filters.begin(); i != m_filters.end(); ++i)
    {
      if ((*i)->supportMime(mime)) return *i;
    };
  return 0;
}

void KigFilters::populate()
{
  new KigFilterKGeo;
  new KigFilterCabri;
}
