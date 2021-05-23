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
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;

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
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  virtual ObjectImp* calc( const LineData& a, const Coordinate& b ) const = 0;
};

#endif
