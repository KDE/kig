// object_type.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "object_type.h"

#include "../misc/i18n.h"

const QCString ObjectType::baseName() const
{
  return mbasetypename;
}

const QString ObjectType::translatedBaseName() const
{
  return i18n( mbasetypename );
}

const QCString ObjectType::fullName() const
{
  return mfulltypename;
}

ObjectType::~ObjectType()
{
}

ObjectType::ObjectType( const char basetypename[],
                        const char fulltypename[],
                        const struct ArgParser::spec argsspec[],
                        int n )
  : mbasetypename( basetypename ), mfulltypename( fulltypename ),
    margsspec( argsspec ), margscount( n )
{
}

bool ObjectType::canMove() const
{
  return false;
}

const struct ArgParser::spec* ObjectType::argsSpec() const
{
  return margsspec;
}

int ObjectType::argsCount() const
{
  return margscount;
}

void ObjectType::move( Object*, const Coordinate&,
                       const Coordinate& ) const
{
}
