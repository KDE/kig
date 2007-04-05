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

#ifndef BOGUS_IMP_H
#define BOGUS_IMP_H

#include "object_imp.h"
#include "../misc/object_hierarchy.h"
#include "../misc/kigtransform.h"

#include <qstring.h>

/**
 * This is the base class for the so-called BogusImp's.  These
 * ObjectImp's are not really ObjectImp's, in that they don't
 * represent objects.  They exist because ObjectImp's also serve
 * another purpose, namely containing data.  They can all be loaded
 * and saved, and the only difference between these objects and normal
 * objects are that these serve *only* to be loaded and saved.  This
 * approach adds a lot of flexibility to the Kig system, and has
 * certainly proven itself very valuable.
 */
class BogusImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
public:
  /**
   * Returns the ObjectImpType representing the BogusImp type.
   */
  static const ObjectImpType* stype();

  Coordinate attachPoint( ) const;
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& w ) const;
  bool inRect( const Rect& r, int width, const KigWidget& w ) const;
  Rect surroundingRect() const;

  ObjectImp* transform( const Transformation& ) const;
};

/**
 * This ObjectImp represents an invalid object.  If a calculation
 * fails, then often an InvalidImp is returned, indicating that the
 * generated object is invalid.
 */
class InvalidImp
  : public BogusImp
{
public:
  /**
   * Returns the ObjectImpType representing the InvalidImp type.
   */
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  /**
   * Construct a new InvalidImp.
   */
  InvalidImp();
  InvalidImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool canFillInNextEscape() const;
  void fillInNextEscape( QString& s, const KigDocument& ) const;

  bool equals( const ObjectImp& rhs ) const;
};

/**
 * This ObjectImp is a BogusImp containing only a double value.
 */
class DoubleImp
  : public BogusImp
{
  double mdata;
public:
  /**
   * Returns the ObjectImpType representing the DoubleImp type.
   */
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  /**
   * Construct a new DoubleImp containing the value d.
   */
  DoubleImp( const double d );

  /**
   * Get hold of the contained data.
   */
  double data() const { return mdata; }
  /**
   * Set the contained data to d.
   */
  void setData( double d ) { mdata = d; }

  DoubleImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool canFillInNextEscape() const;
  void fillInNextEscape( QString& s, const KigDocument& ) const;

  bool equals( const ObjectImp& rhs ) const;
};

/**
 * This ObjectImp is a BogusImp containing only an int value.
 */
class IntImp
  : public BogusImp
{
  int mdata;
public:
  /**
   * Returns the ObjectImpType representing the IntImp type..
   */
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  /**
   * Construct a new IntImp containing the value d.
   */
  IntImp( const int d );

  /**
   * Get hold of the contained data.
   */
  int data() const { return mdata; }
  /**
   * Set the contained data to d.
   */
  void setData( int d )  { mdata = d; }

  IntImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool canFillInNextEscape() const;
  void fillInNextEscape( QString& s, const KigDocument& ) const;

  bool equals( const ObjectImp& rhs ) const;
};

/**
 * This ObjectImp is a BogusImp containing only a string value.
 */
class StringImp
  : public BogusImp
{
  QString mdata;
public:
  /**
   * Returns the ObjectImpType representing the StringImp type..
   */
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  /**
   * Construct a new StringImp containing the string d.
   */
  StringImp( const QString& d );

  /**
   * Get hold of the contained data.
   */
  const QString& data() const { return mdata; }
  /**
   * Set the contained data.
   */
  void setData( const QString& s ) { mdata = s; }

  StringImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool canFillInNextEscape() const;
  void fillInNextEscape( QString& s, const KigDocument& ) const;

  bool equals( const ObjectImp& rhs ) const;
};

class HierarchyImp
  : public BogusImp
{
  ObjectHierarchy mdata;
public:
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  HierarchyImp( const ObjectHierarchy& h );

  const ObjectHierarchy& data() const { return mdata; }
  void setData( const ObjectHierarchy& h ) { mdata = h; }

  HierarchyImp* copy() const;
  const char* baseName() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;
};

/**
 * \internal Don't mistake this imp for something that draws a
 * transformed object.  It does something completely different.  It's
 * a pure data Imp, like DoubleImp and friends that serves only to
 * store the data of a transformation ( see the Transformation class
 * in ../misc/kigtransform.h
 */
class TransformationImp
  : public BogusImp
{
  Transformation mdata;
public:
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  TransformationImp( const Transformation& h );

  const Transformation& data() const { return mdata; }
  void setData( const Transformation& h ) { mdata = h; }

  TransformationImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;
};

class TestResultImp
  : public BogusImp
{
  const QString mdata;
public:
  static const ObjectImpType* stype();
  typedef BogusImp Parent;

  TestResultImp( const QString& s );

  TestResultImp* copy() const;

  const QString& data() const { return mdata; }

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  bool equals( const ObjectImp& rhs ) const;
};

#endif
