// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_KIG_KIG_DOCUMENT_H
#define KIG_KIG_KIG_DOCUMENT_H

#include <set>
#include <vector>

class Coordinate;
class CoordinateSystem;
class KigWidget;
class ObjectHolder;
class ObjectCalcer;
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
  /**
   * Whether to enable visibility of hidden objects.
   */
  bool mnightvision;
  
  /**
   * Specify the number of decimal digits to show
   * when displaying coordinates
   */
  int mcoordinatePrecision;
  
public:
  mutable double mcachedparam;
public:
  KigDocument();
  KigDocument( const std::set<ObjectHolder*> &objects, CoordinateSystem* coordsystem,
               bool showgrid = true, bool showaxes = true, bool nv = false );
  ~KigDocument();

  const CoordinateSystem& coordinateSystem() const;
  bool grid() const;
  bool axes() const;
  bool getNightVision() const;
  bool isUserSpecifiedCoordinatePrecision() const;
  int getCoordinatePrecision() const;
  /**
   * Get a hold of the objects of this KigDocument.
   */
  const std::vector<ObjectHolder*> objects() const;
  const std::set<ObjectHolder*>& objectsSet() const;

  /**
   * sets the coordinate system to \p s , and returns the old one.
   */
  CoordinateSystem* switchCoordinateSystem( CoordinateSystem* s );

  /**
   * sets the coordinate system to \p s , and deletes the old one.
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
   * set to enable/disable night-vision (visibility of hidden objects)
   */
  void setNightVision( bool nv );
  
  void setCoordinatePrecision( int precision );

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

  /**
   * Add the objects \p o to the document.
   */
  void addObject( ObjectHolder* oObject );
  /**
   * Add the objects \p os to the document.
   */
  void addObjects( const std::vector<ObjectHolder*>& os);
  /**
   * Remove the object \p o from the document.
   */
  void delObject( ObjectHolder* o );
  /**
   * Remove the objects \p os from the document.
   */
  void delObjects( const std::vector<ObjectHolder*>& os );
  /**
   * Return all the points that belong (by construction) on both the
   * given curves.  This is useful when the user asks for an intersection
   * point between a conic and a line (or two circles) and one is already
   * there.
   */
  std::vector<ObjectCalcer*> findIntersectionPoints( const ObjectCalcer* c1,
                                                     const ObjectCalcer* c2) const;
};

#endif
