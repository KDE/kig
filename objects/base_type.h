// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_BASE_TYPE_H
#define KIG_OBJECTS_BASE_TYPE_H

#include "object_type.h"

#include "../misc/argsparser.h"

class LineData;

class ObjectABType
  : public ArgsParserObjectType
{
protected:
  ObjectABType( const char* fulltypename, const ArgsParser::spec* argsspec, int n );
  ~ObjectABType();
public:
  ObjectImp* calc( const Args& args, const KigDocument& ) const override;
  bool canMove( const ObjectTypeCalcer& o ) const override;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const override;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const override;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const override;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const override;

  // mp: calcx was an overloaded calc, which caused a compilation warning
  virtual ObjectImp* calcx( const Coordinate& a, const Coordinate& b ) const = 0;
};

class ObjectLPType
  : public ArgsParserObjectType
{
protected:
  ObjectLPType( const char* fullname, const ArgsParser::spec* spec, int n );
  ~ObjectLPType();
public:
  ObjectImp* calc( const Args& args, const KigDocument& ) const override;

  virtual ObjectImp* calc( const LineData& a, const Coordinate& b ) const = 0;
};

#endif
