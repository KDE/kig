#ifndef KGEO_H
#define KGEO_H

#include "filter.h"

#include <vector>

class ObjectHierarchy;
class Object;

class KSimpleConfig;

class KigFilterKGeo
  : public KigFilter
{
public:
  KigFilterKGeo();
  ~KigFilterKGeo();
  virtual bool supportMime ( const QString mime );
  virtual Result convert ( const QString from, KTempFile& to);
protected:
  Result loadMetrics (KSimpleConfig* );
  Result loadObjects (KSimpleConfig* );
  ObjectHierarchy* hier;
  vector<Object*> objs;

  int xMax;
  int yMax;
};

#endif
