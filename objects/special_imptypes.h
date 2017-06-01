// Copyright (C)  2005  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#ifndef KIG_OBJECTS_SPECIAL_IMPTYPES_H
#define KIG_OBJECTS_SPECIAL_IMPTYPES_H

#include "object_imp.h"

/*
 * (mp)
 * special_imptype(s) are a way to identify transversal classes
 * of objects, such as e.g. circles/lines/segments/rays to be
 * used for circular inversion.
 *
 * getDoubleFromImp:  is used to get a double from various
 * different Imp(s): DoubeImp, NumericTextImp, SegmentImp (length)
 *
 * class LengthImpType:
 * is used to match different Imp(s) upon construction
 *
 * lengthimptypeinstance: is just an instance of such class
 *
 * Similarly for AngleImpType and WeightImpType
 * angleimptypeinstance: is just an instance of AngleImpType class
 * weightimptypeinstance: is just an instance of WeightImpType class
 *
 */

double getDoubleFromImp( const ObjectImp*, bool& );

class LengthImpType
  : public ObjectImpType
{
public:
  LengthImpType( const ObjectImpType* parent, const char* internalname );
  ~LengthImpType();
  bool match( const ObjectImpType* t ) const Q_DECL_OVERRIDE;
};

extern LengthImpType lengthimptypeinstance;

class AngleImpType
  : public ObjectImpType
{
public:
  AngleImpType( const ObjectImpType* parent, const char* internalname );
  ~AngleImpType();
  bool match( const ObjectImpType* t ) const Q_DECL_OVERRIDE;
};

extern AngleImpType angleimptypeinstance;

class WeightImpType
  : public ObjectImpType
{
public:
  WeightImpType( const ObjectImpType* parent, const char* internalname );
  ~WeightImpType();
  bool match( const ObjectImpType* t ) const Q_DECL_OVERRIDE;
};

extern WeightImpType weightimptypeinstance;

/*
 * The "InvertibleImpType" class, inherited fron ObjectImpType
 * is used to recognize non-point objects that we know how to
 * circular-invert, namely: lines, segments, rays, circles, arcs.
 * The trick is done by redefining the "match" method of the
 * class in order to return "true" if the argument is one of the
 * types above.
 * This same trick could also be used for TransportOfMeasure in
 * order to allow us to inherit from ArgsParserObjectType instead of
 * directly from ObjectType.
 */

class InvertibleImpType
  : public ObjectImpType
{
public:
  InvertibleImpType( const ObjectImpType* parent, const char* internalname );
  ~InvertibleImpType();
  bool match( const ObjectImpType* t ) const Q_DECL_OVERRIDE;
};

extern InvertibleImpType invertibleimptypeinstance;

#endif
