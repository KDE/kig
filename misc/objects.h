#ifndef OBJECTS_CPP
#define OBJECTS_CPP

#include <qrect.h>
#include <qptrlist.h>
#include <qpainter.h>

#include <kdebug.h>

#include <assert.h>

class Object;

// a list of pointers to objects, with some convenience functions...
class Objects
  : public QPtrList<Object>
{
public:
  typedef QPtrListIterator<Object> iterator;
  // deletes all objects
  void deleteAll();

  bool empty() { return isEmpty(); };

  Objects& operator|=( const Objects& os)
  {
    Object* tmp;
    for (iterator i(os); (tmp = i.current()); ++i)
      add(tmp);
    return *this;
  };

  // adds all objects in objs
  Objects operator|( const Objects& os) const
  {
    Objects tmp;
    Object* o;
    for (iterator i(os); (o = i.current()) ; ++i) tmp.add(o);
    return tmp;
  };

  // removes all objects not in objs
  Objects operator& (const Objects& os) const
  {
    Objects tmp;
    Object* o;
    for (iterator i(os); (o = i.current()); ++i) if (os.contains(o)) tmp.append(o);
    return tmp;
  };
  
  // add o if we don't contain it already;  if you don't need this
  // check, use append
  void add (Object* o)
  {
    if (!contains(o)) append(o);
  };
  
  // get the combined span of all our objects
  QRect getSpan() const;
  
  // convenience: call calc() on all objects...
  // TODO: replace Objects::calc() with ObjectHierarchy::calc() ?
  void calc() const;
  
};

#endif
