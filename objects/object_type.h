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
  const ArgParser margsparser;
  ObjectType( const char fulltypename[],
              const struct ArgParser::spec margsspec[],
              int n, int any = 0 );
public:
  virtual ~ObjectType();

  enum {
    ID_ConstrainedPointType
  };
  virtual bool inherits( int type ) const;

  virtual ObjectImp* calc( const Args& parents ) const = 0;

  virtual bool canMove() const;
  virtual void move( RealObject* ourobj, const Coordinate& from,
                     const Coordinate& dist ) const;

  const char* fullName() const;

  const ArgParser& argsParser() const;
};

#endif
