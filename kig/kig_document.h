// kig_document.h
// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_KIG_KIG_DOCUMENT_H
#define KIG_KIG_KIG_DOCUMENT_H

#include <set>
#include <vector>

class Coordinate;
class CoordinateSystem;
class KigWidget;
class ObjectHolder;
class Rect;

class KigDocument {
  /**
   * Here we keep the objects in the document.
   */
  std::set<ObjectHolder*> mobjects;

  /**
   * The CoordinateSystem as the user sees it: this has little to do
   * with the internal coordinates of the objects... In fact, it's
   * not so different from an object itself ( uses KigPainter to draw
   * itself too...).
   */
  CoordinateSystem* mcoordsystem;
public:
  KigDocument();
  KigDocument( std::set<ObjectHolder*> objects, CoordinateSystem* coordsystem );
  ~KigDocument();

  const CoordinateSystem& coordinateSystem() const;
  // these are the objects that the user is aware of..
  const std::vector<ObjectHolder*> objects() const;
  const std::set<ObjectHolder*> objectsSet() const;

  /**
   * sets the coordinate system to s, and returns the old one..
   */
  CoordinateSystem* switchCoordinateSystem( CoordinateSystem* s );

  /**
   * sets the coordinate system to s, and deletes the old one..
   */
  void setCoordinateSystem( CoordinateSystem* s );

  // what objects are under point p
  std::vector<ObjectHolder*> whatAmIOn( const Coordinate& p, const KigWidget& w ) const;

  std::vector<ObjectHolder*> whatIsInHere( const Rect& p, const KigWidget& );

  // a rect containing most of the objects, which would be a fine
  // suggestion to mapt to the widget...
  Rect suggestedRect() const;

  void addObject( ObjectHolder* oObject );
  void addObjects( const std::vector<ObjectHolder*>& os);
  void delObject( ObjectHolder* o );
  void delObjects( const std::vector<ObjectHolder*>& os );
};

#endif
