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
    ID_CustomType,

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

/**
 * Custom types are types that are not builtin.  Some of their data
 * needs to be saved along with documents.  They are not necessarily
 * user defined types ( well, the user didn't explicitly define them
 * ).
 * E.g.: LocusType can not depend on the parent relationships of its
 * arguments, so it saves an ObjectHierarchy.  This has the added
 * benefit that the hierarchy doesn't need to be recalculated, and
 * that its parent relationships are correct wrt. moving.  This means
 * however that LocusType is a CustomType, since a different instance
 * of it is created for every locus constructed..  MacroType is a
 * CustomType too..
 */
class CustomType
  : public ArgparserObjectType
{
  typedef ObjectType Parent;
public:
  CustomType( const char fulltypename[],
              const struct ArgParser::spec margsspec[],
              int n );

  ~CustomType();

  bool inherits( int type ) const;
};

#endif
