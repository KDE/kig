// coordproppoint.h
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


#ifndef COORDPROPPOINT_H
#define COORDPROPPOINT_H

#include "point.h"

/**
 * This type is a point that gets its information from one of the
 * properties of another object.  It is not user visible in the sense
 * that there is no button in any toolbar for it.  It is created
 * e.g. by right-clicking on a conic, and selecting "Construct first
 * focus".  This prevents me from having to write a separate object
 * type for that..
 */
class CoordinatePropertyPoint
  : public Point
{
public:
  CoordinatePropertyPoint( Object* o, uint propindex );
  CoordinatePropertyPoint( Object* o, const QCString& propstring );
  CoordinatePropertyPoint( const Objects& os );
  CoordinatePropertyPoint( const CoordinatePropertyPoint& p );
  ~CoordinatePropertyPoint();

  prop_map getParams();
  void setParams( const prop_map& m );

  virtual const QCString vFullTypeName() const;
  static const QCString sFullTypeName();
  const QString vDescriptiveName() const;
  const QString vDescription() const;
  const QCString vIconFileName() const;
  const int vShortCut() const;

  virtual Objects getParents() const;
  virtual void calcForWidget( const KigWidget& w );
  virtual void calc();

protected:
  Object* mparent;
  uint mpropindex;
};

#endif
