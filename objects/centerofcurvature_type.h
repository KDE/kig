// Copyright (C) 2004  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#ifndef KIG_OBJECTS_CENTEROFCURVATURE_TYPE_H
#define KIG_OBJECTS_CENTEROFCURVATURE_TYPE_H

#include "base_type.h"

/**
 * the center of curvature of a conic at a point
 */
class CocConicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  CocConicType();
  ~CocConicType();
public:
  static const CocConicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

/**
 * the center of curvature of a cubic at a point
 */
class CocCubicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  CocCubicType();
  ~CocCubicType();
public:
  static const CocCubicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

/**
 * the center of curvature of a curve at a point
 */
class CocCurveType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  CocCurveType();
  ~CocCurveType();
public:
  static const CocCurveType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
