// object_imp.h
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

#ifndef OBJECT_IMP_H
#define OBJECT_IMP_H

#include "common.h"

typedef unsigned int uint;

/**
 * The ObjectImp class represents the behaviour of an object after it
 * is calculated.   This means how to draw() it, whether it claims to
 * contain a certain point etc.  It is also the class where the
 * ObjectType's get their information from..
 */
class ObjectImp
{
public:
  ObjectImp();
  virtual ~ObjectImp();

  virtual ObjectImp* transform( const Transformation& ) const = 0;

  virtual void draw( KigPainter& p ) const = 0;
  virtual bool contains( const Coordinate& p, int width, const KigWidget& si ) const = 0;
  virtual bool inRect( const Rect& r, int width, const KigWidget& si ) const = 0;
  virtual bool valid() const;

  virtual const uint numberOfProperties() const = 0;
  // the names of the properties as perceived by the user..  put
  // I18N_NOOP's around them here..
  virtual const QCStringList properties() const = 0;
  // the names of the properties as known only by kig internally.  No
  // need for I18N_NOOP.  Preferably choose some lowercase name with
  // only letters and dashes, no spaces..
  virtual const QCStringList propertiesInternalNames() const = 0;
  virtual ObjectImp* property( uint which, const KigDocument& d ) const = 0;
  // sometimes we need to know which type an imp needs to be at least
  // in order to have the imp with number which.  Macro's need it
  // foremost.  This function answers that question
  virtual int impRequirementForProperty( uint which ) const = 0;

  enum {
    // we guarantee that "more specialized" types are lower in the list
    // than "less specialized".  E.g. ID_LineImp means: a ray, a line
    // or a segment.  Therefore, ID_SegmentImp has a higher value than
    // ID_LineImp..

    // matches any object type..
    ID_AnyImp = 0,

    // Pseudo-object imp types, these are only used as fixed args for
    // various object types..
    ID_IntImp,
    ID_DoubleImp,
    ID_StringImp,
    ID_InvalidImp,
    ID_HierarchyImp,

    // two id's that match multiple types of imp's..
    ID_CurveImp,
    ID_LineImp,

    // Real object types..
    ID_PointImp,
    ID_TextImp,
    ID_AngleImp,
    ID_VectorImp,
    ID_LocusImp,
    ID_CircleImp,
    ID_ConicImp,
    ID_CubicImp,
    ID_SegmentImp,
    ID_RayImp
  };

  // this translates an id ( e.g. ID_AnyImp ) to its name ( e.g. "any" )
  static const char* idToString( int id );
  static QString translatedName( int id );
  // this translates an id ( e.g. ID_SegmentImp ) to a translated
  // string of the form "Select this %1" ( e.g. "Select this segment"
  // ).
  static const char* selectStatement( int id );

  static QString removeAStatement( int id );
  static QString addAStatement( int id );

  // this translates a string ( e.g. "any" ) to its name (
  // e.g. ID_AnyImp )
  static int stringToID( const QCString& string );

  // we declare this abstract, but we provide an implementation
  // anyway.. This simply means we demand every subclass to implement
  // it, but it can call our implementation if it wants to..
  virtual bool inherits( int typeID ) const = 0;

  // this gives the bottom-most "id" of this imp.
  virtual int id() const = 0;

  virtual const char* baseName() const = 0;

  virtual ObjectImp* copy() const = 0;

  // QString is a string with at least one escape ( "%N" where N is a
  // number ) somewhere.  This function replaces the first escape it
  // sees with the "value" of this imp.  This is e.g. used by TextType
  // to turn its variable args into strings..
  // it's ok not to implement this if it's not going to be used for
  // the imp in question, standard implementation does an assert(
  // false );
  virtual void fillInNextEscape( QString& s, const KigDocument& ) const;
};

#endif
