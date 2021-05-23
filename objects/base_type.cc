// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "base_type.h"

#include "point_imp.h"
#include "line_imp.h"
#include "bogus_imp.h"
#include "object_calcer.h"

#include "../misc/common.h"

ObjectABType::ObjectABType( const char* fulltypename, const ArgsParser::spec* spec, int n )
  : ArgsParserObjectType( fulltypename, spec, n )
{
}

ObjectABType::~ObjectABType()
{
}

ObjectImp* ObjectABType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) )
    return new InvalidImp;

  Coordinate a = static_cast<const PointImp*>( parents[0] )->coordinate();
  Coordinate b = static_cast<const PointImp*>( parents[1] )->coordinate();

  return calcx( a, b );
}

bool ObjectABType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
/*
 * as observed by domi: this object is actually movable also
 * if one point is FreelyTranslatable and the other is
 * only movable, but then the "move" itself requires some
 * trickery.
 */
}

bool ObjectABType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() && parents[1]->isFreelyTranslatable();
}

void ObjectABType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  const Coordinate dist = b - a;
  if ( parents[0]->canMove() )
    parents[0]->move( to, d );
  if ( parents[1]->canMove() )
    parents[1]->move( to + dist, d );
}

ObjectLPType::ObjectLPType( const char* fullname, const ArgsParser::spec* spec, int n )
  : ArgsParserObjectType( fullname, spec, n )
{
}

ObjectLPType::~ObjectLPType()
{
}

ObjectImp* ObjectLPType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;
  LineData l = static_cast<const AbstractLineImp*>( args[0] )->data();
  Coordinate c = static_cast<const PointImp*>( args[1] )->coordinate();
  return calc( l, c );
}

const Coordinate ObjectABType::moveReferencePoint( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> ObjectABType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[1]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}
