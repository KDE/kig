#include "cabri.h"

KigFilterCabri::KigFilterCabri()
{
    
}

KigFilterCabri::~KigFilterCabri()
{
    
}

bool KigFilterCabri::supportMime( const QString mime )
{
  if (mime == QString::fromLatin1( "application/x-cabri" ) )
    return true;
  return false;
}

Result KigFilterCabri::convert( const QString from, KTempFile& to)
{

  return NotSupported;
}
