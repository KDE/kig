#ifndef FILTER_H
#define FILTER_H

#include <ktempfile.h>

#include <qstring.h>

#include <vector>

class KigFilter;

class KigFilters
{
public:
  KigFilters() { sThis = this; populate(); };
  static KigFilters* sThis;
protected:
  typedef vector<KigFilter*> vect;
  static vect m_filters;
public:
  static void add (KigFilter* f) {m_filters.push_back(f); };
  static KigFilter* find (const QString& mime);
  static void populate();
};

class KigFilter
{
public:
  KigFilter() { KigFilters::sThis->add(this); };
  virtual ~KigFilter() {};

  typedef enum { OK, FileNotFound, ParseError, NotSupported } Result;

  // can the filter handle this mimetype ?
  // (a filter only handles _one_ mimetype
  virtual bool supportMime ( const QString /*mime*/ ) { return false; };
  // convert file file to a temporary file (created with KTempFile) in
  // the native Kig Format...
  virtual Result convert ( const QString from, KTempFile& to) = 0;
};
#endif
