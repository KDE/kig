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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "other_type.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "locus_imp.h"

#include "../misc/common.h"
#include "../misc/calcpaths.h"
#include "../misc/goniometry.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"

#include <functional>
#include <algorithm>
#include <cmath>

using std::find;

static const struct ArgsParser::spec argsspecLocus[] =
{
  { HierarchyImp::stype(), "hierarchy", "SHOULD NOT BE SEEN", false },
  { CurveImp::stype(), "curve", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( LocusType )

LocusType::LocusType()
  : ArgsParserObjectType( "Locus", argsspecLocus, 2 )
{
}

LocusType::~LocusType()
{
}

ObjectImp* LocusType::calc( const Args& args, const KigDocument& ) const
{
  using namespace std;

  assert( args.size() >= 2 );
  const Args firsttwo( args.begin(), args.begin() + 2 );
  Args fixedargs( args.begin() + 2, args.end() );

  if ( ! margsparser.checkArgs( firsttwo ) ) return new InvalidImp;
  for ( Args::iterator i = fixedargs.begin(); i != fixedargs.end(); ++i )
    if ( ! (*i)->valid() )
      return new InvalidImp;

  const ObjectHierarchy& hier =
    static_cast<const HierarchyImp*>( args[0] )->data();
  const CurveImp* curveimp = static_cast<const CurveImp*>( args[1] );

  return new LocusImp( curveimp->copy(), hier.withFixedArgs( fixedargs ) );
}

bool LocusType::inherits( int type ) const
{
  return type == ID_LocusType ? true : Parent::inherits( type );
}

const ObjectImpType* LocusType::resultId() const
{
  return LocusImp::stype();
}

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CopyObjectType )

CopyObjectType::CopyObjectType()
  : ObjectType( "Copy" )
{
}

CopyObjectType::~CopyObjectType()
{
}

CopyObjectType* CopyObjectType::instance()
{
  static CopyObjectType t;
  return &t;
}

bool CopyObjectType::inherits( int ) const
{
  return false;
}

ObjectImp* CopyObjectType::calc( const Args& parents, const KigDocument& ) const
{
  assert( parents.size() == 1 );
  return parents[0]->copy();
}

const ObjectImpType* CopyObjectType::impRequirement( const ObjectImp*, const Args& ) const
{
  return ObjectImp::stype();
}

const ObjectImpType* CopyObjectType::resultId() const
{
  // we don't know what we return..
  return ObjectImp::stype();
}

const ObjectImpType* LocusType::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  assert( parents.size() >= 2 );
  Args firsttwo( parents.begin(), parents.begin() + 2 );
  if ( o == parents[0] || o == parents[1] )
    return margsparser.impRequirement( o, firsttwo );
  else
  {
    const HierarchyImp* h = dynamic_cast<const HierarchyImp*>( parents[0] );
    if (  h )
    {
      PointImp* p = new PointImp( Coordinate() );
      Args hargs( parents.begin()+ 2, parents.end() );
      hargs.push_back( p );
      ArgsParser hparser = h->data().argParser();
      const ObjectImpType* ret = hparser.impRequirement( o, hargs );
      delete p;
      return ret;
    }
    else
      return ObjectImp::stype();
  };
}

const LocusType* LocusType::instance()
{
  static const LocusType t;
  return &t;
}

std::vector<ObjectCalcer*> CopyObjectType::sortArgs( const std::vector<ObjectCalcer*>& os ) const
{
  assert( os.size() == 1 );
  return os;
}

std::vector<ObjectCalcer*> LocusType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  assert( args.size() >= 2 );
  std::vector<ObjectCalcer*> firsttwo( args.begin(), args.begin() + 2 );
  firsttwo = margsparser.parse( firsttwo );
  std::copy( args.begin() + 2, args.end(), std::back_inserter( firsttwo ) );
  return firsttwo;
}

Args LocusType::sortArgs( const Args& args ) const
{
  assert( args.size() >= 2 );
  Args firsttwo( args.begin(), args.begin() + 2 );
  firsttwo = margsparser.parse( firsttwo );
  std::copy( args.begin() + 2, args.end(), std::back_inserter( firsttwo ) );
  return firsttwo;
}

Args CopyObjectType::sortArgs( const Args& args ) const
{
  assert( args.size() == 1 );
  return args;
}

bool CopyObjectType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  // TODO: vragen aan parent ?
  // TODO: translate the above TODO ?
  return false;
}

