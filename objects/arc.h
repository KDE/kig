// arc.h
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

#ifndef ARC_H
#define ARC_H

#include "object.h"

#include "../misc/rect.h"

#include <qpointarray.h>

/**
 * This class is inspired upon one in KGeo.  Some code comes from
 * there too..  Thanks to Marc Bartsch <marc.bartsch@web.de>.
 */
class Arc
  : public Object
{
public:
  Arc( const Objects& os );
  Arc( const Arc& a );
  Arc* copy();
  ~Arc();

  Arc* toArc();
  const Arc* toArc() const;

  // some type information
  const QCString vBaseTypeName() const;
  static QCString sBaseTypeName();
  const QCString vFullTypeName() const;
  static QCString sFullTypeName();
  const QString vDescriptiveName() const;
  static const QString sDescriptiveName();
  const QString vDescription() const;
  static const QString sDescription();
  const QCString vIconFileName() const;
  static const QCString sIconFileName();
  const int vShortCut() const;
  static const int sShortCut();
  static const char* sActionName();

  bool contains (const Coordinate& o, const double fault ) const;
  void draw ( KigPainter& p, bool showSelection ) const;

  bool inRect (const Rect&) const;

  // arguments
  static void sDrawPrelim ( KigPainter&, const Objects& o );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );

  Objects getParents() const;

  // moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();

  void calc( const ScreenInfo& );

  // this returns the size of this arc ( in radians, in the range 0 <
  // x < 2*M_PI... )
  double size();

protected:
  Point* mpts[3];
  std::vector<Coordinate> marrow;
  Rect mr;
  // both of these arcs are kept in radians, and converted when
  // necessary...
  double mstartangle;
  double manglelength;
};

#endif
