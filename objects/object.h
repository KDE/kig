// object.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_OBJECTS_OBJECT_H
#define KIG_OBJECTS_OBJECT_H

#include <map>
#include <vector>
#include <qcolor.h>
#include "common.h"

using std::vector;

/**
 * The Object class is an ABC that represents a separate piece of the
 * Kig document.  This includes the user-perceived RealObjects, but
 * also the DataObjects.  The first are the objects that the user
 * sees as objects (lines, circles, points), they do _not_ contain any
 * data, they calc their data from their parents..  The latter are
 * simpler Objects, their only job is to serve as data holders, and
 * some sort of sentinels in the dependency graph.  They are the only
 * ones to really store data.
 */
class Object
{
  Objects mchildren;
  Object( const Object& o );
public:
  Object();
  virtual ~Object();

  enum {
    ID_RealObject,
    ID_DataObject,
    ID_PropertyObject
  };

  virtual bool inherits( int type ) const;
  virtual bool isInternal() const = 0;

  virtual const ObjectImp* imp() const = 0;
  virtual Objects parents() const = 0;

  virtual void draw( KigPainter& p, bool showSelection ) const = 0;
  virtual bool contains( const Coordinate& p, const ScreenInfo& si ) const = 0;
  virtual bool inRect( const Rect& r ) const = 0;

  virtual bool canMove() const = 0;
  virtual void move( const Coordinate& from, const Coordinate& dist ) = 0;

  virtual void calc( const KigDocument& ) = 0;

  virtual bool shown() const = 0;

  virtual void setShown( bool s );

  virtual void setSelected( bool s );

  // these are simply forwarded to the ObjectImp...
  // check the documentation of that class to see what these are
  // for..

  // -> ObjectImp::inherits()
  bool hasimp( int type ) const;

  bool valid() const;

  const uint numberOfProperties() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;

  // every kind of object can have children, and there is no easier
  // way of knowing which they are than to store their pointers.
  // Thus, it is the Object class's job to keep track of them..
  const Objects& children() const;
  const Objects getAllChildren() const;

  void addChild( Object* o );
  void delChild( Object* o );

  virtual void addParent( Object* o );
  virtual void delParent( Object* o );
  virtual void setParents( const Objects& parents );
};

class ObjectWithParents
  : public Object
{
  ObjectWithParents( const ObjectWithParents& o );
protected:
  Objects mparents;
  ObjectWithParents( const Objects& parents );
  ~ObjectWithParents();
public:
  void addParent( Object* o );
  void delParent( Object* o );
  void setParents( const Objects& parents );
  Objects parents() const;

  void calc( const KigDocument& );
  virtual void calc( const Args& a, const KigDocument& ) = 0;
};

/**
 * The Object class represents a geometric object.  This class
 * contains some information about the object, like its parents,
 * color, selected state etc., and pointers to an @ref ObjectType and
 * @ref ObjectImp.  Please check their respective documentation to see
 * what they're about..
 */
class RealObject
  : public ObjectWithParents
{
  QColor mcolor;
  bool mselected;
  bool mshown;
  int mwidth;

  const ObjectType* mtype;
  ObjectImp* mimp;

  RealObject( const RealObject& o );

  void calc( const Args& a, const KigDocument& );

public:
  RealObject( const ObjectType* type, const Objects& parents );
  ~RealObject();

  bool inherits( int type ) const;
  bool isInternal() const;

  const ObjectImp* imp() const;

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;

  bool canMove() const;
  void move( const Coordinate& from, const Coordinate& dist );

  void calc( const KigDocument& d );


  const ObjectType* type() const;
  void setImp( ObjectImp* i );
  void reset( const ObjectType* t, const Objects& parents );
  void setType( const ObjectType* t );

  QColor color() const { return mcolor; };
  void setColor( const QColor c ) { mcolor = c; };

  bool selected() const { return mselected; };
  void setSelected( bool s );

  bool shown() const;
  void setShown( bool shown );

  int width() const { return mwidth; };
  void setWidth( int width ) { mwidth = width; };
};

/**
 * DataObject is an Object class that is _not_ user visible, and only
 * serves to hold data from which RealObjects can calc() themselves.
 */
class DataObject
  : public Object
{
  ObjectImp* mimp;
public:
  DataObject( ObjectImp* imp );
  ~DataObject();

  bool inherits( int type ) const;
  bool isInternal() const;
  void setImp( ObjectImp* imp );

  const ObjectImp* imp() const;
  Objects parents() const;

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;

  bool canMove() const;
  void move( const Coordinate& from, const Coordinate& dist );

  void calc( const KigDocument& );

  bool shown() const;
};

class PropertyObject
  : public Object
{
  ObjectImp* mimp;
  Object* mparent;
  int mpropid;
public:
  PropertyObject( Object* parent, int id );
  ~PropertyObject();

  bool inherits( int type ) const;
  bool isInternal() const;

  const ObjectImp* imp() const;
  Objects parents() const;

  const Object* parent() const;
  int propId() const;

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;

  bool canMove() const;
  void move( const Coordinate& from, const Coordinate& dist );

  void calc( const KigDocument& );

  bool shown() const;
};

#endif
