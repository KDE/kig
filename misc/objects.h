/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


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
