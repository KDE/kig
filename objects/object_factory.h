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
public:

  static ObjectFactory* instance();

  // this returns a fixed point.  Note that the returned object is
  // not added to the document..
  Object* fixedPoint( const Coordinate& c );

  // this returns a constrained point. Note that the returned object
  // is not added to the document..
  Object* constrainedPoint( Object* curve, double param );
  // @overload, changes nothing to the semantics, only calcs the param
  // value for you..
  Object* constrainedPoint( Object* curve, const Coordinate& c,
                                const KigDocument& );

  // this returns a "sensible point".
  // By a "sensible point", I mean a point that would be about what
  // the user expects when he asks for a point at point c.  This is a
  // constrained point if c is on a curve, and otherwise a fixed
  // point.  I might add the possibility for an intersection point
  // sometime.. Note that the returned object is not added to
  // the document..
  Object* sensiblePoint( const Coordinate& c,
                         const KigDocument& d,
                         const KigWidget& w
    );

  // set point to what sensiblePoint would have returned..
  void redefinePoint( Object* point, const Coordinate& c,
                      KigDocument& d, const KigWidget& w );

  // return a locus, defined by the two points ( one constrained, and
  // one following ) in parents.  The semantics of LocusType are a bit
  // weird ( but I believe correct :) ), so this function takes care
  // of the complication there..
  Object* locus( const Objects& parents );

  // returns a label with text s at point c..  It ( and its parents )
  // is calced already...
  Object* label( const QString& s, const Coordinate& loc,
                 bool needframe,
                 const Objects& parents,
                 const KigDocument& doc );

  // this one does the same as the above, only that the new label is
  // attached to locationparent if it is non-null..
  Object* attachedLabel( const QString& s,
                 Object* locationparent,
                 const Coordinate& loc,
                 bool needframe,
                 const Objects& parents,
                 const KigDocument& doc );

  // returns a property object for the property p of object o.  NOTE
  // that o should have already been calc'd, or this will fail and
  // return 0..  The returned object also needs to be calced after
  // this..
  Object* propertyObject( Object* o, const char* p );
};

#endif
