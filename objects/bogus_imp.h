// bogus_imp.h
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

#ifndef BOGUS_IMP_H
#define BOGUS_IMP_H

#include "object_imp.h"
#include "../misc/object_hierarchy.h"
#include "../misc/kigtransform.h"

#include <qstring.h>

/**
 * These ObjectImp's are not really in that they don't represent
 * objects.  They exist because ObjectImp's also serve another
 * purpose, namely containing data.  They can be loaded and saved, and
 * the Object class contains some fixedArgs, that are fixed for the
 * object at hand ( e.g. the Coordinates of a fixed point ).  These
 * ObjectImp's are pure data, and serve only to be loaded and saved..
 */
class BogusImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
public:
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& w ) const;
  bool inRect( const Rect& r, int width, const KigWidget& w ) const;
  bool valid() const;

  ObjectImp* transform( const Transformation& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  int impRequirementForProperty( uint which ) const;
};

class InvalidImp
  : public BogusImp
{
  typedef BogusImp Parent;
public:
  InvalidImp();
  bool valid() const;
  bool inherits( int t ) const;
  InvalidImp* copy() const;
  const char* baseName() const;

  int id() const;
  void visit( ObjectImpVisitor* vtor ) const;

  void fillInNextEscape( QString& s, const KigDocument& ) const;
};

class DoubleImp
  : public BogusImp
{
  typedef BogusImp Parent;
  double mdata;
public:
  DoubleImp( const double d );

  double data() const { return mdata; };
  void setData( double d ) { mdata = d; };

  bool inherits( int typeID ) const;

  DoubleImp* copy() const;
  const char* baseName() const;

  int id() const;
  void visit( ObjectImpVisitor* vtor ) const;

  void fillInNextEscape( QString& s, const KigDocument& ) const;
};

class IntImp
  : public BogusImp
{
  typedef BogusImp Parent;
  int mdata;
public:
  IntImp( const int d );

  int data() const { return mdata; };
  void setData( int d )  { mdata = d; }

  bool inherits( int typeID ) const;
  IntImp* copy() const;
  const char* baseName() const;

  int id() const;
  void visit( ObjectImpVisitor* vtor ) const;

  void fillInNextEscape( QString& s, const KigDocument& ) const;
};

class StringImp
  : public BogusImp
{
  typedef BogusImp Parent;
  QString mdata;
public:
  StringImp( const QString& d );

  const QString& data() const { return mdata; };
  void setData( const QString& s ) { mdata = s; }

  bool inherits( int typeID ) const;
  StringImp* copy() const;
  const char* baseName() const;

  int id() const;
  void visit( ObjectImpVisitor* vtor ) const;

  void fillInNextEscape( QString& s, const KigDocument& ) const;
};

class HierarchyImp
  : public BogusImp
{
  typedef BogusImp Parent;
  ObjectHierarchy mdata;
public:
  HierarchyImp( const ObjectHierarchy& h );

  const ObjectHierarchy& data() const { return mdata; };
  void setData( const ObjectHierarchy& h ) { mdata = h; };

  bool inherits( int type ) const;
  HierarchyImp* copy() const;
  const char* baseName() const;

  int id() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

/**
 * Don't mistake this imp for something that draws a transformed
 * object.  It does something completely different.  It's a pure data
 * Imp, like DoubleImp and friends that serves only to store the data
 * of a transformation ( see the Transformation class in
 * ../misc/kigtransform.h
 */
class TransformationImp
  : public BogusImp
{
  typedef BogusImp Parent;
  Transformation mdata;
public:
  TransformationImp( const Transformation& h );

  const Transformation& data() const { return mdata; };
  void setData( const Transformation& h ) { mdata = h; };

  bool inherits( int type ) const;
  TransformationImp* copy() const;
  const char* baseName() const;

  int id() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

#endif
