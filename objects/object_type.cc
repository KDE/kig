// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "object_type.h"

#include "bogus_imp.h"
#include "object_type_factory.h"

#include "../misc/coordinate.h"

#include <qstringlist.h>

#include <klocale.h>

const char* ObjectType::fullName() const
{
  return mfulltypename;
}

ObjectType::~ObjectType()
{
}

ObjectType::ObjectType( const char fulltypename[] )
  : mfulltypename( fulltypename )
{
  ObjectTypeFactory::instance()->add( this );
}

bool ObjectType::canMove( const ObjectTypeCalcer& ) const
{
  return false;
}

bool ObjectType::isFreelyTranslatable( const ObjectTypeCalcer& ) const
{
  return false;
}

void ObjectType::move( ObjectTypeCalcer&, const Coordinate&, const KigDocument& ) const
{
  // we can't do an assert here, because sometimes ( like in
  // ObjectABType::move, ObjectType::move is called without checking
  // the object's canMove().
//   assert( false );
}

bool ObjectType::inherits( int ) const
{
  return false;
}

ArgsParserObjectType::ArgsParserObjectType( const char fulltypename[],
                                            const struct ArgsParser::spec argsspec[],
                                            int n )
  : ObjectType( fulltypename ), margsparser( argsspec, n )
{
}

const ObjectImpType* ArgsParserObjectType::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  return margsparser.impRequirement( o, parents );
}

const ArgsParser& ArgsParserObjectType::argsParser() const
{
  return margsparser;
}

bool ObjectType::isTransform() const
{
  return false;
}

QStringList ObjectType::specialActions() const
{
  return QStringList();
}

void ObjectType::executeAction( int, ObjectHolder&, ObjectTypeCalcer&, KigPart&, KigWidget&,
                                NormalMode& ) const
{
  assert( false );
}

const Coordinate ObjectType::moveReferencePoint( const ObjectTypeCalcer& ) const
{
  assert( false );
  return Coordinate::invalidCoord();
}

std::vector<ObjectCalcer*> ArgsParserObjectType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return margsparser.parse( args );
}

Args ArgsParserObjectType::sortArgs( const Args& args ) const
{
  return margsparser.parse( args );
}

std::vector<ObjectCalcer*> ObjectType::movableParents( const ObjectTypeCalcer& ) const
{
  return std::vector<ObjectCalcer*>();
}

bool ArgsParserObjectType::isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const
{
  return margsparser.isDefinedOnOrThrough( o, parents );
}

