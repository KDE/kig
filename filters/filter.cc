#include "filter.h"

#include "kgeo.h"

KigFilters::vect KigFilters::m_filters;

KigFilter* KigFilters::find(const QString& mime)
{
  if (m_filters.empty()) populate();
  for (vect::iterator i = m_filters.begin(); i != m_filters.end(); ++i)
    {
      if ((*i)->supportMime(mime)) return *i;
    };
  return 0;
}

// populate our internal list of filters...
void KigFilters::populate()
{
  m_filters.push_back(new KigFilterKGeo);
}
