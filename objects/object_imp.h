// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_OBJECT_IMP_H
#define KIG_OBJECTS_OBJECT_IMP_H

#include "common.h"

class IntImp;
class DoubleImp;
class StringImp;
class InvalidImp;
class HierarchyImp;
class TransformationImp;
class TestResultImp;
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
class FilledPolygonImp;
class ClosedPolygonalImp;
class OpenPolygonalImp;
class BezierImp;
class RationalBezierImp;

/**
 * \internal This is some OO magic commonly referred to as "double
 * dispatch". If you need to do some action on an ObjectImp, and you
 * need to do something different dependent on the type of o, then
 * make a Visitor class that inherits this interface, and implements
 * the appropriate functions properly, and call "o->visit( my_visitor
 * );".
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
  virtual void visit( const TestResultImp* imp );
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
  virtual void visit( const FilledPolygonImp* imp );
  virtual void visit( const ClosedPolygonalImp* imp );
  virtual void visit( const OpenPolygonalImp* imp );
  virtual void visit( const BezierImp* imp );
  virtual void visit( const RationalBezierImp* imp );
};

typedef unsigned int uint;

/**
 * Instances of this class represent a certain ObjectImp type.  Every
 * ObjectImp type has a static ObjectImpType member, that it returns a
 * reference to in its type() function.  Think of it as a nice enum,
 * that you can also get some data from.
 */
class ObjectImpType
{
  const ObjectImpType* mparent;
  const char* minternalname;
  const char* mtranslatedname;
  const char* mselectstatement;
  const char* mselectnamestatement;
  const char* mremoveastatement;
  const char* maddastatement;
  const char* mmoveastatement;
  const char* mattachtothisstatement;
  const char* mshowastatement;
  const char* mhideastatement;
  class StaticPrivate;
  static StaticPrivate* sd();
public:
  /**
   * Returns the type with name n.
   *
   * \internal Do *not* call this from functions that can be called at
   * static initializer time !  It depends on information that is only
   * available after that stage and will crash if used too early.
   */
  static const ObjectImpType* typeFromInternalName( const char* n );

  /**
   * \internal Construct an ObjectImpType, with a lot of data about
   * your ObjectImp type.
   *
   * @param parent is the ObjectImpType of
   * your parent ObjectImp type. Never give 0 as parent, except for
   * the top ObjectImp ObjectImpType.
   * @param internalname is an internal name
   * @param translatedname is a translatable string like "segment"
   * @param selectstatement is a translatable string like "Select this segment"
   * @param selectnamestatement is a translatable string like "Select segment %1"
   * @param removeastatement is a translatable string like "Remove a Segment"
   * @param addastatement is a translatable string like "Add a Segment"
   * @param moveastatement is a translatable string like "Move a Segment"
   * @param attachtothisstatement is a translatable string like "Attach to
   *     this segment"
   * @param showastatement is a translatable string like "Show a Segment"
   * @param hideastatement is a translatable string like "Hide a Segment"
   *
   * All translatable strings should have
   * I18N_NOOP around them!
   */
  explicit ObjectImpType(
    const ObjectImpType* parent, const char* internalname,
    const char* translatedname,
    const char* selectstatement,
    const char* selectnamestatement,
    const char* removeastatement,
    const char* addastatement,
    const char* moveastatement,
    const char* attachtothisstatement,
    const char* showastatement,
    const char* hideastatement );
  virtual ~ObjectImpType();

  /**
   * Does the ObjectImp type represented by this instance inherit the
   * ObjectImp type represented by t ?
   */
  bool inherits( const ObjectImpType* t ) const;
  virtual bool match( const ObjectImpType* t ) const;

  /**
   * Returns an internal name for this ObjectImp type.  This name is
   * guaranteed unique, and mostly corresponds with the class name of
   * the corresponding ObjectImp.
   */
  const char* internalName() const;
  /**
   * The name of this type, translated to the currently used language.
   */
  QString translatedName() const;
  /**
   * Returns a translatable string of the form "Select this %1".
   * E.g. "Select this segment".  Note that users of this function
   * should use i18n on the returned string before using it.
   */
  const char* selectStatement() const;

  /**
   * Returns a translatable string of the form "Select point %1".  %1
   * will be filled in by whomever calls this function with the name
   * of the object in question.  This function should be used as
   * follows: i18n( x->selectNameStatement() ).arg( xname ).
   */
  const char* selectNameStatement() const;

  /**
   * Returns a translated string of the form "Remove a xxx".
   * E.g. "Remove a Segment".
   */
  QString removeAStatement() const;
  /**
   * Returns a translated string of the form "Add a xxx".
   * E.g. "Add a Segment".
   */
  QString addAStatement() const;
  /**
   * Returns a translated string of the form "Move a xxx".
   * E.g. "Move a Segment".
   */
  QString moveAStatement() const;
  /**
   * Returns a translated string of the form "Attach to this xxx".
   * E.g. "Attach to this segment".
   * \internal This is used by the text label construction mode
   */
  QString attachToThisStatement() const;

  /**
   * Returns a translated string of the form "Show a xxx".
   * E.g. "Show a Segment".
   */
  QString showAStatement() const;

  /**
   * Returns a translated string of the form "Hide a xxx".
   * E.g. "Hide a Segment".
   */
  QString hideAStatement() const;
};

/**
 * The ObjectImp class represents the behaviour of an object after it
 * is calculated.   This means how to draw() it, whether it claims to
 * contain a certain point etc.  It is also the class where the
 * ObjectType's get their information from.
 */
class ObjectImp
{
protected:
  ObjectImp();
public:
  /**
   * The ObjectImpType representing the base ObjectImp class.  All
   * other ObjectImp's inherit from this type.
   */
  static const ObjectImpType* stype();

  /**
   * mp: The following three methods (getPropLid, getPropGid, getPropName)
   * deal with the properties of ObjectImp(s).
   * the properties architecture (vectors of properties associated to
   * each Imp) didn't work at all in situation when a node in the object
   * hierarchy of which a properties is used changes it's Imp dynamically.
   * An example of this is a python script that constructs a "hyperbolic 
   * segment" in the half-plane model of hyperbolic geometry, that results
   * in a circular arc or a segment depending on the relative position of
   * the two end-points.
   * A workaround for this problem is to maintain two numberings for a
   * property: the "local" id (Lid) is the index in the property vector
   * associated to an ObjectImp, it makes only sense relatively to a
   * specific ObjectImp; the "global" id (Gid) is a global numbering
   * generated runtime and distinguishing properties only based on their
   * internal name.
   * A property node must only use the global numbering, so that when the
   * parent changes it's Imp, the correct property (if it exists) of the
   * new Imp will be used.
   * the association of Gid to properties is constructed runtime whenever
   * a new property is first used by populating a static vector
   * (see object_imp.cc).
   * The conversion Gid->Lid requires a vector search, so that a caching
   * mechanism has been set up in the ObjectPropertyCalcer that stores the
   * Lid and recalculates it when the typeid of the ObjectImp of the parent
   * changes.
   *
   * getPropLid: returns the local numbering corresponding to a Gid
   * getPropGid: returns the Gid of a property given its internal name
   * getPropName: returns the internal name of a property given its Gid
   *
   * Note: in object_hierarchy.cc the class "FetchPropertyNode" is quite
   * similar to "ObjectPropertyCalcer", and thus is similarly restructured.
   * However the caching mechanism has not been setup in this case. 
   * It seems that "FetchPropertyNode" is only used for the "drawprelim"
   * in constructions that involve a macro that in turn involves a property,
   * so perhaps the caching is not that important.
   */
  int getPropLid( int propgid ) const;
  int getPropGid( const char *pname ) const;
  const char* getPropName( int propgid ) const;

  virtual ~ObjectImp();

  /**
   * Returns true if this ObjectImp inherits the ObjectImp type
   * represented by t.
   * E.g. you can check whether an ObjectImp is a LineImp by doing:
   * \if creating-python-scripting-doc
   * \code
   * if object.inherits( LineImp.stype() ):
   * \endcode
   * \else
   * \code
   * if( object.inherits( LineImp::stype() )
   * \endcode
   * \endif
   */
  bool inherits( const ObjectImpType* t ) const;

  /**
   * Returns a reference point where to attach labels; when this
   * returns an invalidCoord then the attachment is either not
   * done at all, or done in a specific way (like for curves,
   * or for points) The treatment of points could also take
   * advantage of this attachment mechanism.
   *
   * If this method returns a valid Coordinate, then this is
   * interpreted as a pivot point for the label, which can still
   * be moved relative to that point, but follows the object when
   * the object changes.
   * In practice a new RelativePointType is created (position of
   * the string), this type in turn depends on the object (to get
   * its attachPoint) and two DoubleImp that are interpreted as
   * relative displacement (x and y)
   */
  virtual Coordinate attachPoint( ) const = 0;

  /**
   * Return this ObjectImp, transformed by the transformation t.
   */
  virtual ObjectImp* transform( const Transformation& t ) const = 0;

  virtual void draw( KigPainter& p ) const = 0;
  virtual bool contains( const Coordinate& p, int width,
                         const KigWidget& si ) const = 0;
  virtual bool inRect( const Rect& r, int width,
                       const KigWidget& si ) const = 0;
  virtual Rect surroundingRect() const = 0;

  /**
   * Returns true if this is a valid ObjectImp.
   * If you want to return an invalid ObjectImp, you should return an
   * InvalidImp instance.
   */
  bool valid() const;

  virtual int numberOfProperties() const;
  // the names of the properties as perceived by the user..  put
  // I18N_NOOP's around them here..
  virtual const QByteArrayList properties() const;
  // the names of the properties as known only by kig internally.  No
  // need for I18N_NOOP.  Preferably choose some lowercase name with
  // only letters and dashes, no spaces..
  virtual const QByteArrayList propertiesInternalNames() const;
  virtual ObjectImp* property( int which, const KigDocument& d ) const;
  // Sometimes we need to know which type an imp needs to be at least
  // in order to have the imp with number which.  Macro's need it
  // foremost.  This function answers that question..
  virtual const ObjectImpType* impRequirementForProperty( int which ) const;
  // Return whether the property with number which is by construction
  // always a point on this curve ( if this is a curve ), or always a
  // curve through this point ( if this is a curve ).
  virtual bool isPropertyDefinedOnOrThroughThisImp( int which ) const;
  // What icon should be shown when talking about this property ?
  virtual const char* iconForProperty( int which ) const;

  /**
   * Returns the lowermost ObjectImpType that this object is an
   * instantiation of.
   * E.g. if you want to get a string containing the internal name of
   * the type of an object, you can do:
   * \if creating-python-scripting-doc
   * \code
   * tn = object.type().internalName()
   * \endcode
   * \else
   * \code
   * std::string typename = object.type()->internalName();
   * \endcode
   * \endif
   */
  virtual const ObjectImpType* type() const = 0;
  virtual void visit( ObjectImpVisitor* vtor ) const = 0;

  /**
   * Returns a copy of this ObjectImp.
   * The copy is an exact copy.  Changes to the copy don't affect the
   * original.
   */
  virtual ObjectImp* copy() const = 0;

  // s is a string with at least one escape ( "%N" where N is a
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
   * Returns true if this ObjectImp is equal to rhs.
   * This function checks whether rhs is of the same ObjectImp type,
   * and whether it contains the same data as this ObjectImp.
   * \internal It is used e.g. by the KigCommand stuff to see what the
   * user has changed during a move.
   */
  virtual bool equals( const ObjectImp& rhs ) const = 0;

  /**
   * \internal Return true if this imp is just a cache imp.  This
   * means that it will never be considered to be stored in a file or
   * in an ObjectHierarchy.  This is useful for objects which cannot
   * (easily and usefully) be (de)serialized, like e.g.
   * PythonCompiledScriptImp.  For normal objects, the default
   * implementation returns false, which is fine.
   */
  virtual bool isCache() const;
};
#endif
