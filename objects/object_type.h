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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_OBJECTS_OBJECT_TYPE_H
#define KIG_OBJECTS_OBJECT_TYPE_H

#include "common.h"
#include "../misc/argsparser.h"

class ObjectTypeCalcer;

/**
 * The ObjectType class is a thing that represents the "behaviour" for
 * a certain type..  This basically means that it decides what
 * \ref ObjectImp the object gets in the calc() function, how the
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
    ID_LocusType,
    ID_FixedPointType
  };

  virtual bool inherits( int type ) const;

  virtual ObjectImp* calc( const Args& parents, const KigDocument& d ) const = 0;

  virtual bool canMove( const ObjectTypeCalcer& ourobj ) const;
  virtual bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const;
  virtual std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  virtual const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const;
  virtual void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
                     const KigDocument& d ) const;

  const char* fullName() const;

  /**
   * Supposing that \p parents would be given as parents to
   * this type's calc function, this function returns the ObjectImp id
   * that \p o should at least have..  ( \p o should be part of \p parents )
   */
  virtual const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const = 0;

  /**
   * Supposing that \p parents would be given as parents to this type's
   * calc function, this function returns whether the returned
   * ObjectImp will be, by construction, on \p o ( if \p o is a curve ), or
   * through \p o ( if \p o is a point ).
   */
  virtual bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const = 0;

  /**
   * returns the ObjectImp id of the ObjectImp's produced by this
   * ObjectType..  if the ObjectType can return different sorts of
   * ObjectImp's, it should return the biggest common id, or
   * ID_AnyImp..
   */
  virtual const ObjectImpType* resultId() const = 0;

  virtual std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const = 0;

  virtual Args sortArgs( const Args& args ) const = 0;

  /**
   * is this object type a transformation type.  We want to know this
   * cause transform types are shown separately in an object's RMB
   * menu..
   */
  virtual bool isTransform() const;

  // ObjectType's can define some special actions, that are strictly
  // specific to the type at hand.  E.g. a text label allows to toggle
  // the display of a frame around the text.  Constrained and fixed
  // points can be redefined etc.

  /**
   * return i18n'd names for the special actions..
   */
  virtual QStringList specialActions() const;
  /**
   * execute the \p i 'th action from the specialActions above..
   */
  virtual void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& t,
                              KigPart& d, KigWidget& w, NormalMode& m ) const;
};

/**
 * This is a convenience subclass of ObjectType that a type should
 * inherit from if its parents can be specified in an ArgsParser..
 */
class ArgsParserObjectType
  : public ObjectType
{
protected:
  const ArgsParser margsparser;
  ArgsParserObjectType( const char fulltypename[],
                        const struct ArgsParser::spec argsspec[],
                        int n );
public:
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  const ArgsParser& argsParser() const;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;
};

#endif
