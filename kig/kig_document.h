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

/**
 * KigDocument is the class holding the real data in a Kig document.
 * It owns a set of Objects, and a CoordinateSystem, which comprise
 * the entire content of a Kig document.
 */
class KigDocument {
  /**
   * Here we keep the objects in the document.  These are only the
   * objects that the user is aware of.  Other objects exist as well,
   * but there's no ObjectHolder for them, and they only exist because
   * some other ObjectCalcer has them as its ancestor.
   */
  std::set<ObjectHolder*> mobjects;

  /**
   * The CoordinateSystem as the user sees it: this has little to do
   * with the internal coordinates of the objects...  It really serves
   * to translate user coordinates from and to internal coordinates.
   */
  CoordinateSystem* mcoordsystem;
  /**
   * Whether to show the grid.
   */
  bool mshowgrid;
  /**
   * Whether to show the axes.
   */
  bool mshowaxes;
public:
  KigDocument();
  KigDocument( std::set<ObjectHolder*> objects, CoordinateSystem* coordsystem,
               bool showgrid = true, bool showaxes = true );
  ~KigDocument();

  /**
   * Get a hold of the objects and coordinate system of this
   * KigDocument.
   */
  const CoordinateSystem& coordinateSystem() const;
  const bool grid() const;
  const bool axes() const;
  const std::vector<ObjectHolder*> objects() const;
  const std::set<ObjectHolder*>& objectsSet() const;

  /**
   * sets the coordinate system to s, and returns the old one..
   */
  CoordinateSystem* switchCoordinateSystem( CoordinateSystem* s );

  /**
   * sets the coordinate system to s, and deletes the old one..
   */
  void setCoordinateSystem( CoordinateSystem* s );

  /**
   * set to show/hide the grid.
   */
  void setGrid( bool showgrid );

  /**
   * set to show/hide the grid.
   */
  void setAxes( bool showaxes );

  /**
   * Return a vector of objects that contain the given point.
   */
  std::vector<ObjectHolder*> whatAmIOn( const Coordinate& p, const KigWidget& w ) const;

  /**
   * Return a vector of objects that are in the given Rect.
   */
  std::vector<ObjectHolder*> whatIsInHere( const Rect& p, const KigWidget& );

  /**
   * Return a rect containing most of the objects, which would be a
   * fine suggestion to map to the widget...
   */
  Rect suggestedRect() const;

  void addObject( ObjectHolder* oObject );
  void addObjects( const std::vector<ObjectHolder*>& os);
  void delObject( ObjectHolder* o );
  void delObjects( const std::vector<ObjectHolder*>& os );
};

#endif
