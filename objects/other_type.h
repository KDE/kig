// other_type.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_OTHER_TYPE_H
#define KIG_MISC_OTHER_TYPE_H

#include "base_type.h"
#include "../misc/object_hierarchy.h"

class AngleType
  : public ArgparserObjectType
{
  AngleType();
  ~AngleType();
public:
  static const AngleType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  void executeAction( int i, RealObject* o, KigDocument& d, KigWidget& w,
                      NormalMode& m ) const;
};

class VectorType
  : public ObjectABType
{
  VectorType();
  ~VectorType();
public:
  static const VectorType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;
};

class LocusType
  : public ArgparserObjectType
{
  typedef ArgparserObjectType Parent;
  LocusType();
  ~LocusType();
public:
  static const LocusType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  const ObjectHierarchy& hierarchy() const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;

  bool inherits( int type ) const;
  const ObjectImpType* resultId() const;
};

class CopyObjectType
  : public ObjectType
{
protected:
  CopyObjectType();
  ~CopyObjectType();
public:
  static CopyObjectType* instance();
  bool inherits( int type ) const;
  ObjectImp* calc( const Args& parents, const KigDocument& d ) const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  const ObjectImpType* resultId() const;
};

/**
 * an arc by a start point, an intermediate point and an end point
 */
class ArcBTPType
  : public ArgparserObjectType
{
  typedef ArgparserObjectType Parent;
  ArcBTPType();
  ~ArcBTPType();
public:
  static const ArcBTPType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;

  bool inherits( int type ) const;
  const ObjectImpType* resultId() const;
};

#endif
