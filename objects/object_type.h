// object_type.h
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

#ifndef KIG_OBJECTS_OBJECT_TYPE_H
#define KIG_OBJECTS_OBJECT_TYPE_H

#include "common.h"
#include "../misc/argsparser.h"

/**
 * The ObjectType class is a thing that represents the "behaviour" for
 * a certain type..  This basically means that it decides what
 * @ref ObjectImp the object gets in the calc() function, how the
 * object move()'s etc.
 */
class ObjectType
{
  const char* mfulltypename;
protected:
  ObjectType( const char fulltypename[] );
public:
  virtual ~ObjectType();

  enum {
    ID_ConstrainedPointType,
    ID_LocusType
  };

  virtual bool inherits( int type ) const;

  virtual ObjectImp* calc( const Args& parents, const KigDocument& d ) const = 0;

  virtual bool canMove() const;
  virtual void move( RealObject* ourobj, const Coordinate& from,
                     const Coordinate& dist, const KigDocument& d ) const;

  const char* fullName() const;

  // Supposing that parents.with( o ) would be given as parents to
  // this type's calc function, this function returns the ObjectImp id
  // that o should at least have..
  virtual int impRequirement( const ObjectImp* o, const Args& parents ) const = 0;

  // returns the ObjectImp id of the ObjectImp's produced by this
  // ObjectType..  if the ObjectType can return different sorts of
  // ObjectImp's, it should return the biggest common id, or
  // ID_AnyImp..
  virtual int resultId() const = 0;

  // is this object type a transformation type.  We want to know this
  // cause transform types are shown separately in an object's RMB
  // menu..
  virtual bool isTransform() const;

  // ObjectType's can define some special actions, that are strictly
  // specific to the type at hand.  E.g. a text label allows to toggle
  // the display of a frame around the text.  Constrained and fixed
  // points can be redefined etc.

  // return i18n'd names for the special actions..
  virtual QStringList specialActions() const;
  // execute the i'th action from the specialActions above..
  virtual void executeAction( int i, RealObject* o, KigDocument& d, KigWidget& w,
                              NormalMode& m ) const;
};

/**
 * This is a convenience subclass of ObjectType that a type should
 * inherit from if its parents can be specified in an argparser..
 */
class ArgparserObjectType
  : public ObjectType
{
protected:
  const ArgParser margsparser;
  ArgparserObjectType( const char fulltypename[],
                       const struct ArgParser::spec argsspec[],
                       int n );
public:
  int impRequirement( const ObjectImp* o, const Args& parents ) const;
  const ArgParser& argParser() const;
};

#endif
