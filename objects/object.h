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
#include <qcolor.h>
#include "common.h"

class NormalModePopupObjects;
class NormalMode;

/**
 * The Object class represents a geometric object.  This class
 * contains some information about the object, like its parents,
 * children, color, selected state etc., and pointers to an @ref
 * ObjectType and @ref ObjectImp.  Please check their respective
 * documentation to see what they're about..
 */
class Object
{
  QColor mcolor;
  bool mselected;
  bool mshown;
  int mwidth;

  const ObjectType* mtype;
  ODP mimp;

  Objects mparents;
  Args mfixedargs;

  Objects mchildren;
public:
  Object( const ObjectType* type, const Objects& parents, const Args& fixedargs );
  Object( const Object& o );
  ~Object();

  const ObjectImp* imp() const { return mimp.get(); };
  const ObjectType* type() const { return mtype; };
  const Args& fixedArgs() const { return mfixedargs; };

  void setImp( ObjectImp* i );

  void reset( const ObjectType* t, const Args& fixedArgs, const Objects& parents );

  double getParam( const Coordinate& c ) const;
  const Coordinate getPoint( double param ) const;

  QCString baseTypeName() const;
  QString translatedBaseTypeName() const;

  // Properties are a generic way to retrieve object information.
  // They are mainly used for showing textlabels with information
  // about an object ( e.g. This segment is %1 units long ).
  // every object has a number of properties.
  // the Object class itself defines some properties too..
  const uint numberOfProperties() const;
  const Property property( uint which, const KigWidget& w ) const;
  // this gives a list of brief descriptions of the properties.  The
  // names should be between I18N_NOOP(), and they should be ordered
  // such that if one takes property( a ) gives you the property
  // described by "properties()[a]".
  const QCStringList properties() const;


  // only object types that have "parameters" need this, a parameter
  // is something which you cannot calculate from your parents,
  // e.g. an independent point's params are its coordinates, a
  // ConstrainedPoint's also got a param..
  // Objects that reimplement this should call Object::getParams() and
  // setParams() after you've handled your params.. --> to set params
  // common to all objects like the color...
//   prop_map saveData ();
//   void loadData ( const prop_map& );

  void draw( KigPainter& p, bool showSelection ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;

  bool canMove() const;
  void move( const Coordinate& from, const Coordinate& dist );

  void calc();

//   void addActions( NormalModePopupObjects& p );
//   void doNormalAction( int which, KigDocument& d, KigWidget& w, NormalMode& m, const Coordinate& cp );
//   void doPopupAction( int popupid, int actionid, KigDocument* d, KigWidget* w, NormalMode* m, const Coordinate& cp );

  QColor color() const { return mcolor; };
  void setColor( const QColor c ) { mcolor = c; };

  bool selected() const { return mselected; };
  void setSelected( bool s ) { mselected = s; };

  bool shown() const { return mshown; };
  void setShown( bool shown ) { mshown = shown; };

  bool valid() const;

  const Objects& children() const { return mchildren; };
  Objects getAllChildren() const;

  const Objects& parents() const;

  bool has( int typeID ) const;

  void addChild( Object* o );
  void delChild( Object* o );
  void delParent( Object* o );
};

#endif
