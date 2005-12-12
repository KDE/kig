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
 * angleimptypeinstance: is just an instance of such class
 *
 */

double getDoubleFromImp( const ObjectImp*, bool& );

class LengthImpType
  : public ObjectImpType
{
public:
  LengthImpType( const ObjectImpType* parent, const char* internalname,
    const char* translatedname,
    const char* selectstatement,
    const char* selectnamestatement,
    const char* removeastatement,
    const char* addastatement,
    const char* moveastatement,
    const char* attachtothisstatement,
    const char* showastatement,
    const char* hideastatement );
  ~LengthImpType();
  virtual bool match( const ObjectImpType* t ) const;
};

extern LengthImpType lengthimptypeinstance;

class AngleImpType
  : public ObjectImpType
{
public:
  AngleImpType( const ObjectImpType* parent, const char* internalname,
    const char* translatedname,
    const char* selectstatement,
    const char* selectnamestatement,
    const char* removeastatement,
    const char* addastatement,
    const char* moveastatement,
    const char* attachtothisstatement,
    const char* showastatement,
    const char* hideastatement );
  ~AngleImpType();
  virtual bool match( const ObjectImpType* t ) const;
};

extern AngleImpType angleimptypeinstance;

#endif
