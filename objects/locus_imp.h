// locus_imp.h
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

#ifndef KIG_OBJECTS_LOCUS_IMP_H
#define KIG_OBJECTS_LOCUS_IMP_H

#include "curve_imp.h"
#include "../misc/object_hierarchy.h"

class LocusImp
  : public CurveImp
{
  typedef CurveImp Parent;
  CurveImp* mcurve;
  const ObjectHierarchy mhier;
public:
  LocusImp( CurveImp*, const ObjectHierarchy& );
  ~LocusImp();
  LocusImp* copy() const;
  const char* baseName() const;

  bool inherits( int type ) const;

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const ScreenInfo& si ) const;
  bool inRect( const Rect& r, int width, const ScreenInfo& si ) const;
  bool valid() const;

  // TODO ?
  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;

  double getParam( const Coordinate& point ) const;
  const Coordinate getPoint( double param, const KigDocument& ) const;

  const CurveImp* curve() const;
  const ObjectHierarchy& hierarchy() const;

  int id() const;
};

#endif
