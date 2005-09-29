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

#ifndef KIG_OBJECTS_INVERSION_TYPE_H
#define KIG_OBJECTS_INVERSION_TYPE_H

#include "base_type.h"

/**
 * Inversion of a point, line
 */
class InvertPointType
  : public ArgsParserObjectType
{
  InvertPointType();
  ~InvertPointType();
public:
  static const InvertPointType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class InvertLineType
  : public ArgsParserObjectType
{
  InvertLineType();
  ~InvertLineType();
public:
  static const InvertLineType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class InvertSegmentType
  : public ArgsParserObjectType
{
  InvertSegmentType();
  ~InvertSegmentType();
public:
  static const InvertSegmentType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class InvertCircleType
  : public ArgsParserObjectType
{
  InvertCircleType();
  ~InvertCircleType();
public:
  static const InvertCircleType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class InvertArcType
  : public ArgsParserObjectType
{
  InvertArcType();
  ~InvertArcType();
public:
  static const InvertArcType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
