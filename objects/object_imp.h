// object_imp.h
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

#ifndef OBJECT_IMP_H
#define OBJECT_IMP_H

#include "common.h"

typedef unsigned int uint;

/**
 * The ObjectImp class represents the behaviour of an object after it
 * is calculated.   This means how to draw() it, whether it claims to
 * contain a certain point etc.  It is also the class where the
 * ObjectType's get their information from..
 */
class ObjectImp
{
public:
  ObjectImp();
  virtual ~ObjectImp();

  virtual ObjectImp* transform( const Transformation& ) const = 0;

  virtual void draw( KigPainter& p ) const = 0;
  virtual bool contains( const Coordinate& p, const ScreenInfo& si ) const = 0;
  virtual bool inRect( const Rect& r ) const = 0;
  virtual bool valid() const;

  virtual const uint numberOfProperties() const = 0;
  virtual const QCStringList properties() const = 0;
  virtual const Property property( uint which, const KigWidget& w ) const = 0;

  enum {
    // Pseudo-object imp types, these are only used as fixed args for
    // various object types..
    ID_DoubleImp = 0,
    ID_IntImp,
    ID_StringImp,

    // Real object types..
    ID_PointImp,
    ID_CurveImp,
    ID_LineImp,
    ID_LabelImp,
    ID_AngleImp,
    ID_VectorImp,
    ID_LocusImp,
    ID_CircleImp,
    ID_ConicImp,
    ID_CubicImp,
    ID_SegmentImp,
    ID_RayImp,

    // matches any object type..
    ID_AnyImp
  };

  // we declare this abstract, but we provide an implementation
  // anyway.. This simply means we demand every subclass to implement
  // it, but it can call our implementation if it wants to..
  virtual bool inherits( int typeID ) const = 0;

  virtual ObjectImp* copy() const = 0;

  virtual const char* baseName() const = 0;
  QString translatedBaseName() const;
};

#endif
