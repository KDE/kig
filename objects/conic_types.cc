// conic_types.cc
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

#include "conic_types.h"

#include "bogus_imp.h"
#include "conic_imp.h"
#include "point_imp.h"
#include "../misc/conic-common.h"

static const struct ArgParser::spec argsspec5p[] =
{
  { ObjectImp::ID_PointImp, 5 }
};

ConicB5PType::ConicB5PType()
  : ObjectType( "conic", "ConicB5P", argsspec5p, 1 )
{
}

ConicB5PType::~ConicB5PType()
{
}

ObjectImp* ConicB5PType::calc(
  const Args& parents,
  const KigWidget& ) const
{
  assert( parents.size() <= 5 );
  std::vector<Coordinate> points;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  if ( points.empty() ) return new InvalidImp;
  else return new ConicImpCart(
    calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry ) );
}

const ConicB5PType* ConicB5PType::instance()
{
  static const ConicB5PType t;
  return &t;
}
