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

#include "curve_imp.h"
#include "../misc/coordinate.h"

const ObjectImpType* CurveImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "curve",
    I18N_NOOP( "curve" ),
    I18N_NOOP( "Select this curve" ),
    I18N_NOOP( "Select curve %1" ),
    I18N_NOOP( "Remove a Curve" ),
    I18N_NOOP( "Add a Curve" ),
    I18N_NOOP( "Move a Curve" ),
    I18N_NOOP( "Attach to this curve" ),
    I18N_NOOP( "Show a Curve" ),
    I18N_NOOP( "Hide a Curve" )
    );
  return &t;
}

Coordinate CurveImp::attachPoint() const
{
  return Coordinate::invalidCoord();
}
