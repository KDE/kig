// tangent_type.h
// Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_OBJECTS_TANGENT_TYPE_H
#define KIG_OBJECTS_TANGENT_TYPE_H

#include "base_type.h"

/**
 * the line tangent to a generic conic
 */
class TangentConicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  TangentConicType();
  ~TangentConicType();
public:
  static const TangentConicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

/**
 * the line tangent to an arc
 */
class TangentArcType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  TangentArcType();
  ~TangentArcType();
public:
  static const TangentArcType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
