// transform_types.cc
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

#include "transform_types.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "line_imp.h"
#include "other_imp.h"
#include "../misc/coordinate.h"
#include "../misc/kigtransform.h"

static const ArgParser::spec argsspecav[] =
{
  { ObjectImp::ID_AnyImp, 1 },
  { ObjectImp::ID_VectorImp, 1 }
};

TranslatedType::TranslatedType()
  : ObjectType( "TranslatedType", argsspecav, 2 )
{
}

TranslatedType::~TranslatedType()
{
}

const TranslatedType* TranslatedType::instance()
{
  static const TranslatedType t;
  return &t;
}

ObjectImp* TranslatedType::calc( const Args& args ) const
{
  if ( args.size() != 2 ) return new InvalidImp;
  assert( args[1]->inherits( ObjectImp::ID_VectorImp ) );

  Coordinate dir = static_cast<const VectorImp*>( args[1] )->dir();
  Transformation t = Transformation::translation( dir );
  return args[0]->transform( t );
}

static const ArgParser::spec argsspecap[] =
{
  { ObjectImp::ID_AnyImp, 1 },
  { ObjectImp::ID_PointImp, 1 }
};

PointReflectionType::PointReflectionType()
  : ObjectType( "PointReflection", argsspecap, 2 )
{
}

PointReflectionType::~PointReflectionType()
{
}

const PointReflectionType* PointReflectionType::instance()
{
  static const PointReflectionType t;
  return &t;
}

ObjectImp* PointReflectionType::calc( const Args& args ) const
{
  if ( args.size() != 2 ) return new InvalidImp;
  assert( args[1]->inherits( ObjectImp::ID_PointImp ) );

  Coordinate center = static_cast<const PointImp*>( args[1] )->coordinate();
  Transformation t = Transformation::pointReflection( center );
  return args[0]->transform( t );
}

static const ArgParser::spec argsspecal[] =
{
  { ObjectImp::ID_AnyImp, 1 },
  { ObjectImp::ID_LineImp, 1 }
};

LineReflectionType::LineReflectionType()
  : ObjectType( "LineReflection", argsspecal, 2 )
{
}

LineReflectionType::~LineReflectionType()
{
}

const LineReflectionType* LineReflectionType::instance()
{
  static const LineReflectionType t;
  return &t;
}

ObjectImp* LineReflectionType::calc( const Args& args ) const
{
  if ( args.size() != 2 ) return new InvalidImp;
  assert( args[1]->inherits( ObjectImp::ID_LineImp ) );

  LineData d = static_cast<const AbstractLineImp*>( args[1] )->data();
  Transformation t = Transformation::lineReflection( d );
  return args[0]->transform( t );
}
