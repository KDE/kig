/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef KIG_MISC_OBJECTS_H
#define KIG_MISC_OBJECTS_H

#include <vector>
#include <algorithm>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

class KigWidget;

// this used to be a class with convenience functions, but i've
// replaced them all with stl stuff...
class Object;

template<class T>
class myvector
  : public std::vector<T>
{
  typedef std::vector<T> parent;
public:
  myvector();
  myvector( uint size, const T& l )
    : parent( size, l ) {};
  template<class Iter> myvector( Iter b, Iter e )
    : std::vector<T>( b, e ) {};

  // constructs a vector with a single element in it...
  explicit myvector( const T& element );

  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;

  // unique push back -> check if we already contain o and push it
  // back otherwise...
  void upush( const T& o );

  void upush( const myvector<T>& os );

  bool contains( const T& o ) const;

  myvector<T>& operator|=( const myvector<T>& os )
    { upush( os ); return *this; };

  // remove all occurences of o..
  void remove( const T& o );

  myvector<T> with( const T& o ) const;
};

template<class T>
myvector<T> myvector<T>::with( const T& o ) const
{
  myvector<T> ret( *this );
  ret.push_back( o );
  return ret;
};

template<class T>
myvector<T>::myvector( const T& t )
{
  push_back( t );
};

template<class T>
myvector<T>::myvector()
{
};

template<class T>
bool myvector<T>::contains( const T& o ) const
{
  return std::find( begin(), end(), o ) != end();
};

template<class T>
void myvector<T>::upush( const T& b )
{
  if ( std::find( begin(), end(), b ) == end() )
    push_back( b );
};

template<class T>
void myvector<T>::upush( const myvector<T>& os )
{
  for ( const_iterator i = os.begin();
        i != os.end(); ++i )
    upush( *i );
};

template<class T>
void myvector<T>::remove( const T& o )
{
  iterator i;
  while ( (i = std::find( begin(), end(), o ) ) != end() )
    erase( i );
};

template<class T>
myvector<T> operator&( const myvector<T>& o, const myvector<T>& s )
{
  myvector<T> result;
  std::set_intersection( o.begin(), o.end(), s.begin(), s.end(),
                       std::back_inserter( result ) );
  return result;
};

template<class T>
myvector<T> operator|( const myvector<T>& o, const myvector<T>& s )
{
  myvector<T> result;
  std::set_union( o.begin(), o.end(), s.begin(), s.end(),
                       std::back_inserter( result ) );
  return result;
};

template<class iter>
void delete_all( iter current, iter end )
{
  for ( ; current != end; ++current )
  {
    delete *current;
  };
};

class Objects
  : public myvector<Object*>
{
public:
  Objects() {};
  Objects( uint n, Object* l = 0 ) : myvector<Object*>( n, l ) {};
  template<class Iter> Objects( Iter b, Iter e ) : myvector<Object*>( b, e ) {};
  Objects( const Objects& os ) : myvector<Object*>( os ) {};
  explicit Objects( Object* const o ) : myvector<Object*>( o ) {};
  void calc( const KigWidget& w ) const;
  Objects with( Object* ) const;
};

#endif
