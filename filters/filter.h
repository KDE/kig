#ifndef FILTER_H
#define FILTER_H

#include <ktempfile.h>

#include <qstring.h>

#include <vector>

class KigFilter;

class KigFilters
{
public:
  static KigFilters* instance() { return sThis ? sThis : sThis = new KigFilters(); };
  KigFilter* find (const QString& mime);

protected:
  KigFilters();
  static KigFilters* sThis;
  typedef std::vector<KigFilter*> vect;
  vect mFilters;
};

class KigFilter
{
public:
  KigFilter() {};
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
