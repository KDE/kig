#ifndef KIG_IFACE_H
#define KIG_IFACE_H

#include <dcopobject.h>

class KigIface : virtual public DCOPObject
{
  K_DCOP
public:
  KigIface();
  ~KigIface();
k_dcop:
  virtual void openURL(const QString& s) = 0;
};

#endif
