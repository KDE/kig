#ifndef OBJECTS_CPP
#define OBJECTS_CPP

#include <list>

#include <qrect.h>
#include <qptrlist.h>
#include <qpainter.h>

class Object;

// a list of pointers to objects, with some convenience functions...
class Objects
: protected std::list<Object*>
{
public:
  typedef std::list<Object*>::iterator iterator;
  typedef std::list<Object*>::const_iterator const_iterator;
  iterator begin() { return std::list<Object*>::begin(); };
  const_iterator begin() const { return std::list<Object*>::begin(); };
  iterator end() { return std::list<Object*>::end();} ;
  const_iterator end() const { return std::list<Object*>::end();} ;
  size_t size() const { return std::list<Object*>::size();} ;
  bool empty() const { return std::list<Object*>::empty();};
  void clear() { std::list<Object*>::clear(); };
  Object* front() const { return std::list<Object*>::front();};
  Object* back() const { return std::list<Object*>::back(); };
 public:
  Objects() {};
  Objects(Object* o) { push_back(o); };
  ~Objects() {};

    // deletes all objects
  void deleteAll();

    // do we contain o ?
  bool contains(const Object* o) const;

    // remove o
  void remove(Object* o);

    // adds all objects in objs
  Objects& operator|=( const Objects& objs);

  // removes all objects not in objs
  Objects operator& (const Objects& objc);

  // add o if we don't contain it already,  if you don't need this check, use push
  void add (Object* o) { if (!contains(o)) push(o); };

    // synonym for add
    Objects& operator+= ( Object* o ) { add( o ); return *this; }

    // add o no matter what...
    void push( Object* o ) {push_back( o );};

    // get the combined span of all our objects
  QRect getSpan() const;

    // draw all our objects, cf. Object::draw
  void draw ( QPainter& p, bool showSelection ) const ;

  // convenience: call calc() on all objects...
  void calc();

  // convenience: get the combined objectOverlayList from our children
  QPtrList<QRect> getOverlay( const QRect& border) const;
};

// this class sorts its objects, which should make it faster for very
// large numbers of objects, but, more importantly, makes sure that
// points are on top of other objects...
class SortedObjects
{
};


#endif
