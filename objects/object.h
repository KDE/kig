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

#include <qcolor.h>
#include "common.h"

class ObjectImpType;

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
    ID_PropertyObject,
    ID_ObjectWithParents
  };

  virtual bool inherits( int type ) const;

  virtual const ObjectImp* imp() const = 0;
  virtual Objects parents() const = 0;
  /**
   * what imp does an object need to have in order to be a valid
   * parent replacing o where os are the other parents..
   * @see ObjectType::impRequirement and
   * ObjectImp::impRequirementForProperty
   */
  virtual const ObjectImpType* impRequirement( Object* o, const Objects& os ) const;

  virtual void draw( KigPainter& p, bool showSelection ) const = 0;
  virtual bool contains( const Coordinate& p, const KigWidget& si ) const = 0;
  virtual bool inRect( const Rect& r, const KigWidget& si ) const = 0;

  virtual bool canMove() const;
  // what point should MovingMode think this object is currently on ?
  // If the user then moves his mouse by (x,y), then move() will be
  // called with to set to moveReferencePoint + (x,y)..
  // a point should just return its current location, a segment should
  // probably return the coordinate of one of its points, or the
  // center or whatever..  just make sure your object moves properly ;)
  virtual const Coordinate moveReferencePoint() const;
  virtual void move( const Coordinate& to, const KigDocument& );

  virtual void calc( const KigDocument& ) = 0;

  virtual bool shown() const = 0;
  virtual void setShown( bool s );

  virtual QColor color() const;
  virtual void setColor( const QColor& c );

  virtual void setSelected( bool s );

  // these are simply forwarded to the ObjectImp...
  // check the documentation of that class to see what these are
  // for..

  // -> ObjectImp::inherits()
  bool hasimp( const ObjectImpType* type ) const;

  bool valid() const;

  const uint numberOfProperties() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  const char* iconForProperty( uint which ) const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;

  // every kind of object can have children, and there is no easier
  // way of knowing which they are than to store their pointers.
  // Thus, it is the Object class's job to keep track of them..
  const Objects children() const;
  const Objects getAllChildren() const;

  void addChild( Object* o );

  void delChild( Object* o );

  // this function sets the parents of this object to parents.  It
  // doesn't do anything for deleting obsolete parents etc. you should
  // do this yourself ( hint: check out the deadParents() function in
  // ../misc/calcpaths.h )
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
  void addParents( const Objects& os );
  void delParent( Object* o );
  void setParents( const Objects& parents );
  Objects parents() const;

  void calc( const KigDocument& );
  virtual void calc( const Args& a, const KigDocument& ) = 0;
};

/**
 * The RealObject class represents an object that is shown on screen.
 * This class contains some information about the object, like its
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

  // c++ doesn't consider our parent's calc function when looking for
  // a calc function with this signature, because we have another calc
  // function..
  void calc( const KigDocument& );

  bool inherits( int type ) const;

  const ObjectImp* imp() const;

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const KigWidget& ) const;
  bool inRect( const Rect& r, const KigWidget& ) const;

  bool canMove() const;
  const Coordinate moveReferencePoint() const;
  void move( const Coordinate& dist, const KigDocument& );

  const ObjectType* type() const;
  void setType( const ObjectType* t );

  QColor color() const;
  void setColor( const QColor& c );

  bool selected() const { return mselected; };
  void setSelected( bool s );

  bool shown() const;
  void setShown( bool shown );

  int width() const { return mwidth; };
  void setWidth( int width );

  const ObjectImpType* impRequirement( Object* o, const Objects& os ) const;
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
  void setImp( ObjectImp* imp );
  /**
   * sets imp as the old imp, and returns the old one.. This exchanges
   * ownership in order to prevent some useless copies and deletions..
   */
  ObjectImp* switchImp( ObjectImp* imp );

  const ObjectImp* imp() const;
  Objects parents() const;

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const KigWidget& ) const;
  bool inRect( const Rect& r, const KigWidget& ) const;

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

  const ObjectImp* imp() const;
  Objects parents() const;

  const Object* parent() const;
  Object* parent();
  int propId() const;

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const KigWidget& ) const;
  bool inRect( const Rect& r, const KigWidget& ) const;

  void calc( const KigDocument& );

  bool shown() const;

  const ObjectImpType* impRequirement( Object* o, const Objects& os ) const;
};

/**
 * This class is not an object.  It only serves to hold a reference to
 * a couple of objects, so they don't get deleted..  you can
 * manipulate the referneced objects using the ObjectWithParents API.
 */
class ReferenceObject
  : public ObjectWithParents
{
public:
  ReferenceObject();
  ReferenceObject( Object* o );
  ReferenceObject( const Objects& os );
  ~ReferenceObject();

  void clearParents();

private:
  const ObjectImp* imp() const;
  void draw( KigPainter&, bool ) const;
  bool inRect( const Rect&, const KigWidget& ) const;
  bool shown() const;
  void calc( const Args&, const KigDocument& );
  bool contains( const Coordinate&, const KigWidget& ) const;
};

#endif
