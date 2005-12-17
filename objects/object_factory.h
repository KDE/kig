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

#ifndef KIG_OBJECTS_OBJECT_FACTORY_H
#define KIG_OBJECTS_OBJECT_FACTORY_H

#include "common.h"

class ObjectFactory
{
public:

  static const ObjectFactory* instance();

  /**
   * this returns a fixed point.  Note that the returned object is
   * not added to the document..
   */
  ObjectHolder* fixedPoint( const Coordinate& c ) const;
  ObjectTypeCalcer* fixedPointCalcer( const Coordinate& c ) const;

  /**
   * this returns a CursorPointType; this is used during special
   * constructions (e.g. regular polygons) where the constructor
   * wants to use the cursor position without actually generating
   * an object depending on a new point there.
   */
  ObjectTypeCalcer* cursorPointCalcer( const Coordinate& c ) const;

  /**
   * this returns a relative point (to an object). Note that the returned object
   * is not added to the document..
   */
  ObjectTypeCalcer* relativePointCalcer( ObjectCalcer* o, const Coordinate& loc ) const;

  /**
   * this returns a constrained point. Note that the returned object
   * is not added to the document..
   */
  ObjectHolder* constrainedPoint( ObjectCalcer* curve, double param ) const;
  ObjectTypeCalcer* constrainedPointCalcer( ObjectCalcer* curve, double param ) const;
  /**
   * \overload, changes nothing to the semantics, only calcs the param
   * value for you..
   */
  ObjectTypeCalcer* constrainedPointCalcer(
    ObjectCalcer* curve, const Coordinate& c, const KigDocument& ) const;
  ObjectHolder* constrainedPoint(
    ObjectCalcer* curve, const Coordinate& c, const KigDocument& ) const;

  /**
   * this returns a "sensible point".
   * By a "sensible point", I mean a point that would be about what
   * the user expects when he asks for a point at point \p c . This is a
   * constrained point if \p c is on a curve, and otherwise a fixed
   * point.  I might add the possibility for an intersection point
   * sometime.. Note that the returned object is not added to
   * the document..
   */
  ObjectTypeCalcer* sensiblePointCalcer(
    const Coordinate& c, const KigDocument& d, const KigWidget& w ) const;
  ObjectHolder* sensiblePoint(
    const Coordinate& c, const KigDocument& d, const KigWidget& w ) const;

  /**
   * set point to what sensiblePoint would have returned..
   */
  void redefinePoint( ObjectTypeCalcer* point, const Coordinate& c,
                      KigDocument& d, const KigWidget& w ) const;

  /**
   * return a locus, defined by the two points ( one constrained, and
   * one following ) \p a and \p b . \p a should be the constrained point,
   * and thus, it has to be of type ObjectTypeCalcer where a->type() is of
   * type ConstrainedPointType.  The semantics of LocusType are a bit
   * weird ( but I believe correct :) ), so this function takes care
   * of the complication there..
   */
  ObjectTypeCalcer* locusCalcer( ObjectCalcer* a, ObjectCalcer* b ) const;
  ObjectHolder* locus( ObjectCalcer* a, ObjectCalcer* b ) const;

  /**
   * returns a label with text \p s at point \p c .. It ( and its parents )
   * is calced already...
   */
  ObjectHolder* label(
    const QString& s, const Coordinate& loc,
    bool needframe, const std::vector<ObjectCalcer*>& parents,
    const KigDocument& doc ) const;
  ObjectTypeCalcer* labelCalcer(
    const QString& s, const Coordinate& loc,
    bool needframe, const std::vector<ObjectCalcer*>& parents,
    const KigDocument& doc ) const;

  /**
   * this one does the same as the above, only that the new label is
   * attached to locationparent if it is non-null..
   */
  ObjectTypeCalcer* attachedLabelCalcer(
    const QString& s, ObjectCalcer* locationparent,
    const Coordinate& loc, bool needframe,
    const std::vector<ObjectCalcer*>& parents,
    const KigDocument& doc ) const;
  /**
   * this has been added because it comes handy when redefining
   * a text label, we move here all the code for getting an
   * attach point from the method above
   */
  ObjectCalcer* getAttachPoint(
    ObjectCalcer* locationparent,
    const Coordinate& loc,
    const KigDocument& doc ) const;
  ObjectHolder* attachedLabel(
    const QString& s, ObjectCalcer* locationparent,
    const Coordinate& loc, bool needframe,
    const std::vector<ObjectCalcer*>& parents,
    const KigDocument& doc ) const;

  /**
   * returns a property object for the property \p p of object \p o .
   *
   * \note
   * \p o should have already been calc'd, or this will fail and
   * return 0..  The returned object also needs to be calced after
   * this..
   */
  ObjectPropertyCalcer* propertyObjectCalcer( ObjectCalcer* o, const char* p ) const;
  ObjectHolder* propertyObject( ObjectCalcer* o, const char* p ) const;
};

#endif
