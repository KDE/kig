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

#ifndef KIG_OBJECTS_OBJECT_IMP_H
#define KIG_OBJECTS_OBJECT_IMP_H

#include "common.h"

class IntImp;
class DoubleImp;
class StringImp;
class InvalidImp;
class HierarchyImp;
class TransformationImp;
class CurveImp;
class LineImp;
class PointImp;
class TextImp;
class AngleImp;
class VectorImp;
class LocusImp;
class CircleImp;
class ConicImp;
class CubicImp;
class SegmentImp;
class RayImp;
class ArcImp;

/**
 * This is some OO magic commonly referred to as "double dispatch".
 * If you need to do some action on an ObjectImp, and you need to do
 * something different dependent on the type of o, then make a Visitor
 * class that inherits this interface, and implements the appropriate
 * functions properly, and call "o->visit( my_visitor );".
 */
class ObjectImpVisitor
{
public:
  virtual ~ObjectImpVisitor();
  void visit( const ObjectImp* imp );
  virtual void visit( const IntImp* imp );
  virtual void visit( const DoubleImp* imp );
  virtual void visit( const StringImp* imp );
  virtual void visit( const InvalidImp* imp );
  virtual void visit( const HierarchyImp* imp );
  virtual void visit( const TransformationImp* imp );
  virtual void visit( const LineImp* imp );
  virtual void visit( const PointImp* imp );
  virtual void visit( const TextImp* imp );
  virtual void visit( const AngleImp* imp );
  virtual void visit( const VectorImp* imp );
  virtual void visit( const LocusImp* imp );
  virtual void visit( const CircleImp* imp );
  virtual void visit( const ConicImp* imp );
  virtual void visit( const CubicImp* imp );
  virtual void visit( const SegmentImp* imp );
  virtual void visit( const RayImp* imp );
  virtual void visit( const ArcImp* imp );
};

typedef unsigned int uint;

/**
 * Instances of this class represent a certain ObjectImp type.  Every
 * ObjectImp type should have a static ObjectImpType member, that it
 * returns a reference to in its type() function..  It's really not
 * much more than a nice enum.
 */
class ObjectImpType
{
  const ObjectImpType* mparent;
  const char* minternalname;
  const char* mtranslatedname;
  const char* mselectstatement;
  const char* mremoveastatement;
  const char* maddastatement;
  const char* mmoveastatement;
  const char* mattachtothisstatement;
  class StaticPrivate;
  static StaticPrivate* sd();
public:
  /**
   * returns the type with name string.  Do *not* call this from
   * functions that can be called at static initializer time !  It
   * depends on information that is only available after that stage
   * and will crash if used too early..
   */
  static const ObjectImpType* typeFromInternalName( const char* string );

  /**
   * Construct an ObjectImpType, with a lot of data about your
   * ObjectImp type.  translatedname is a translatable string like
   * "segment", selectstatement is a translatable string like "Select
   * a Segment", removeastatement is a translatable string like
   * "Remove a Segment", addastatement is a translatable string like
   * "Add a Segment", moveastatement is a translatable string like
   * "Move a Segment".  All translatable strings should have I18N_NOOP
   * around them !
   * @param parent is the ObjectImpType of your parent ObjectImp
   * type.  Never give 0 as parent, except for the top ObjectImp
   * ObjectImpType..
   */
  ObjectImpType( const ObjectImpType* parent, const char* internalname,
                 const char* translatedname, const char* selectstatement,
                 const char* removeastatement, const char* addastatement,
                 const char* moveastatement,
                 const char* attachtothisstatement );
  ~ObjectImpType();

  bool inherits( const ObjectImpType* t ) const;
  //void serialize( const ObjectImp& imp );
  const char* internalName() const;
  QString translatedName() const;
  // returns a translated string of the form "Select this %1" (
  // e.g. "Select this segment" ).
  const char* selectStatement() const;
  // returns a translated string of the form "remove a xxx" (
  // e.g. "Remove a segment" ).
  QString removeAStatement() const;
  // returns a translated string of the form "add a xxx" (
  // e.g. "Add a segment" ).
  QString addAStatement() const;
  // returns a translated string of the form "move a xxx" (
  // e.g. "Move a segment" ).
  QString moveAStatement() const;
  // returns a translated string of the form "attach to this xxx" (
  // e.g. "Attach to this segment" ). ( used by the text label
  // construction mode )
  QString attachToThisStatement() const;
};

/**
 * The ObjectImp class represents the behaviour of an object after it
 * is calculated.   This means how to draw() it, whether it claims to
 * contain a certain point etc.  It is also the class where the
 * ObjectType's get their information from..
 */
class ObjectImp
{
public:
  static const ObjectImpType* stype();
  ObjectImp();
  virtual ~ObjectImp();

  bool inherits( const ObjectImpType* t ) const;

  virtual ObjectImp* transform( const Transformation& ) const = 0;

  virtual void draw( KigPainter& p ) const = 0;
  virtual bool contains( const Coordinate& p, int width,
                         const KigWidget& si ) const = 0;
  virtual bool inRect( const Rect& r, int width,
                       const KigWidget& si ) const = 0;
  bool valid() const;

  virtual const uint numberOfProperties() const;
  // the names of the properties as perceived by the user..  put
  // I18N_NOOP's around them here..
  virtual const QCStringList properties() const;
  // the names of the properties as known only by kig internally.  No
  // need for I18N_NOOP.  Preferably choose some lowercase name with
  // only letters and dashes, no spaces..
  virtual const QCStringList propertiesInternalNames() const;
  virtual ObjectImp* property( uint which, const KigDocument& d ) const;
  // Sometimes we need to know which type an imp needs to be at least
  // in order to have the imp with number which.  Macro's need it
  // foremost.  This function answers that question..
  virtual const ObjectImpType* impRequirementForProperty( uint which ) const;
  // What icon should be shown when talking about this property ?
  virtual const char* iconForProperty( uint which ) const;

  virtual const ObjectImpType* type() const = 0;
  virtual void visit( ObjectImpVisitor* vtor ) const = 0;

  virtual ObjectImp* copy() const = 0;

  // QString is a string with at least one escape ( "%N" where N is a
  // number ) somewhere.  This function replaces the first escape it
  // sees with the "value" of this imp ( using the QString::arg
  // functions ).  This is e.g. used by TextType to turn its variable
  // args into strings..
  // if you implement this, then you should return true in
  // canFillInEscape() ( standard implementation returns false ), and
  // override fillInNextEscape() ( standard implementation does an
  // assert( false ) )..
  virtual bool canFillInNextEscape() const;
  virtual void fillInNextEscape( QString& s, const KigDocument& ) const;

  /**
   * this function checks whether rhs is of the same imp type, and
   * whether it contains the same data.  It is used by the KigCommand
   * stuff to see what the user has changed during a move..
   */
  virtual bool equals( const ObjectImp& rhs ) const = 0;

  /**
   * Return true if this imp is just a cache imp.  This means that it
   * will never be considered to be stored in a file or in an
   * ObjectHierarchy.  This is useful for objects which cannot
   * (easily and usefully) be (de)serialized, like e.g.
   * PythonCompiledScriptImp..  For normal objects, the default
   * implementation returns false, which is fine.
   */
  virtual bool isCache() const;
};
#endif
