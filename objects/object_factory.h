// object_factory.h
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

#ifndef KIG_OBJECTS_OBJECT_FACTORY_H
#define KIG_OBJECTS_OBJECT_FACTORY_H

#include "common.h"

class ObjectFactory
{
  static ObjectFactory* s;
public:

  static ObjectFactory* instance();

  // this returns the necessary objects to show a fixed point.  That
  // is: two DataObject's for the coordinates followed by one
  // RealObject for the point..  Note that the returned objects are
  // not yet added to the document..
  Objects fixedPoint( const Coordinate& c );

  // this returns the necessary objects to show a constrained point.
  // That is: one DataObject for the parameter, and one RealObject for
  // the point..  Note that the returned objects are not yet added to
  // the document..
  Objects constrainedPoint( Object* curve, double param );
  // @overload, changes nothing to the semantics, only calcs the param
  // value for you..
  Objects constrainedPoint( Object* curve, const Coordinate& c, const KigDocument& );

  // this returns the necessary objects to show a sensible point.
  // By a "sensible point", I mean a point that would be about what
  // the user expects when he asks for a point at point c.  This is a
  // constrained point if c is on a curve, and otherwise a fixed
  // point.  I might add the possibility for an intersection point
  // sometime.. Note that the returned objects are not yet added to
  // the document..
  Objects sensiblePoint( const Coordinate& c,
                         const KigDocument& d,
                         const KigWidget& w
    );

  // set point to what sensiblePoint would have returned..  This
  // function returns what fixedPoint() or constrainedPoint() would
  // have returned.  This function takes care of removing old
  // DataObjects, that have no more children, so you don't have to
  // worry about that..  newly created DataObjects are added to the
  // document, so don't worry about that either :)
  Objects redefinePoint( Object* point, const Coordinate& c,
                         KigDocument& d, const KigWidget& w );

  // return a locus, defined by the two points ( one constrained, and
  // one following ) in parents.  The semantics of LocusType are a bit
  // weird ( but I believe correct :) ), so this function takes care
  // of the complication there..
  // The function returns the objects necessary to define the locus:
  // first the hierarchy DataObject, next the locus RealObject..
  Objects locus( const Objects& parents );

  // returns the objects necessary to show a label with text s at
  // point c: first a int dataobject ( indicates whether we want a
  // frame or not ), next a string DataObject, then a point
  // DataObject, and next a label RealObject..
  Objects label( const QString& s, const Coordinate& loc, bool needframe = false,
                 const Objects& parents = Objects() );
};

#endif
