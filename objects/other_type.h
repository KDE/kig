// Copyright (C) 2003-2004  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_OTHER_TYPE_H
#define KIG_MISC_OTHER_TYPE_H

#include "base_type.h"
#include "../misc/object_hierarchy.h"

class LocusType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  LocusType();
  ~LocusType();
public:
  static const LocusType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;

  bool inherits( int type ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;
};

class CopyObjectType
  : public ObjectType
{
protected:
  CopyObjectType();
  ~CopyObjectType();
public:
  static CopyObjectType* instance();
  bool inherits( int type ) const Q_DECL_OVERRIDE;
  ObjectImp* calc( const Args& parents, const KigDocument& d ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;
};

#endif
