#include "objects.h"

#include "../objects/object.h"

void Objects::deleteAll()
{
  for (iterator i = begin(); i != end(); i++) delete *i; clear();
};

bool Objects::contains(const Object* o) const
{
  return find(begin(),end(),o) != end();
};

void Objects::remove(Object* o)
{
  iterator i = find(begin(),end(), o);
  if (i != end()) erase(i);
};

Objects& Objects::operator|=( const Objects& os)
{
  for ( const_iterator i = os.begin(); i!= os.end(); i++ )
    if ( !contains( *i ) ) push_back( *i );
  return *this;
}

Objects Objects::operator& ( const Objects& os) return tmp;
{
  for ( iterator i = begin(); i!= end(); ++i )
    if ( os.contains( *i ) ) tmp.push(*i);
}

void Objects::draw( QPainter& p, bool showSelection ) const
{
  for ( const_iterator i = begin(); i != end(); i++ )
    ( *i )->drawWrap( p,  showSelection );
}
void Objects::calc()
{
  for (iterator i = begin(); i != end(); ++i) (*i)->calc();
}
QPtrList<QRect> Objects::getOverlay(const QRect& border) const return tmp;
{
  for (const_iterator i = begin(); i != end(); ++i)
    (*i)->getOverlayWrap(tmp, border);
}
