// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_OBJECT_CALCER_H
#define KIG_OBJECTS_OBJECT_CALCER_H

#include <typeinfo>
#include "common.h"
#include "../misc/boost_intrusive_pointer.hpp"

class ObjectCalcer;

void intrusive_ptr_add_ref( ObjectCalcer* p );
void intrusive_ptr_release( ObjectCalcer* p );

/**
 * An ObjectCalcer is an object that represents an algorithm for
 * calculating an ObjectImp from other ObjectImp's.  It is also a node
 * in the dependency graph of a certain document. E.g. a LineImp can
 * be calculated from the two PointImp's it has to go through; every
 * time either of them moves, this calculation is redone.  In this
 * case, there would be an ObjectCalcer that keeps a reference to its
 * two parents ( the ObjectCalcer's representing the points ), and
 * that will calculate its ObjectImp value every time it is asked to
 * do so ( i.e. every time one of its parents moves ).
 *
 * Each ObjectHolder keeps its ObjectImp itself, and recalculates it
 * from its parents in its calc() method ( if necessary ).
 *
 * Because of the complex relations that ObjectCalcer's hold to other
 * ObjectCalcer's and to other classes, they have been made
 * reference-counted.  This means that they keep a count internally of
 * how much times a pointer to them is held.  If this count reaches 0,
 * this means that nobody needs them anymore, and they delete
 * themselves.  E.g. an ObjectCalcer always keeps a reference to its
 * parents, to ensure that those aren't deleted before it is deleted.
 *
 * At runtime, there will be an entire graph of ObjectCalcer that
 * depend on their parents.  At the bottom, there are Calcer's that
 * the user is aware of, and that are held by ObjectHolder's.  At the
 * top, there are Calcer's without parents that serve only to hold
 * some data.  Those are most likely ObjectConstCalcer's.  There are
 * some algorithms to work with the dependency graph in various ways
 * in ../misc/calcpath.h
 *
 * ObjectCalcer's also decide how an object should be allowed to
 * move.  If the user selects a point, and tries to move it, then its
 * ObjectCalcer will be asked whether it can move, and if so, will be
 * asked to move.  See below with the canMove(), move() and
 * moveReferencePoint() methods.
 */
class ObjectCalcer
{
protected:
  /**
   * ObjectCalcer's are reference counted.  They all take a reference
   * to their parents, and some other classes like ObjectHolder take a
   * reference to some ObjectCalcer's that they don't want to see
   * deleted.
   */
  friend void intrusive_ptr_add_ref( ObjectCalcer* p );
  friend void intrusive_ptr_release( ObjectCalcer* p );
  int refcount;
  void ref();
  void deref();

  // we keep track of our children, so algorithms can easily walk over
  // the dependency graph..

  std::vector<ObjectCalcer*> mchildren;

  ObjectCalcer();
public:
  /**
   * a calcer should call this to register itself as a child of this
   * calcer.  This automatically takes a reference.
   */
  void addChild( ObjectCalcer* c );
  /**
   * a calcer should call this in its destructor, to inform its parent
   * that it is no longer a child of this calcer.  This will release
   * the reference taken in addChild.
   */
  void delChild( ObjectCalcer* c );

  // use this pointer type to keep a reference to an ObjectCalcer...
  typedef myboost::intrusive_ptr<ObjectCalcer> shared_ptr;

  /**
   * Returns the child ObjectCalcer's of this ObjectCalcer.
   */
 std::vector<ObjectCalcer*> children() const;

  virtual ~ObjectCalcer();
  /**
   * Returns the parent ObjectCalcer's of this ObjectCalcer.
   */
  virtual std::vector<ObjectCalcer*> parents() const = 0;
  /**
   * Returns the ObjectImp of this ObjectCalcer.
   */
  virtual const ObjectImp* imp() const = 0;
  /**
   * Makes the ObjectCalcer recalculate its ObjectImp from its
   * parents.
   */
  virtual void calc( const KigDocument& ) = 0;

  /**
   * An ObjectCalcer expects its parents to have an ObjectImp of a
   * certain type.  This method returns the ObjectImpType that \p o
   * should have. \p os is a list of all the parents in order, and
   * \p o is part of it. This method will return the ObjectImpType
   * that the parent should *at least* be.  For example, a Translated
   * object can translate any sort of object, so it will return
   * ObjectImp::stype() here ( the topmost ObjectImpType, that all
   * other ObjectImpType's inherit ).
   */
  virtual const ObjectImpType* impRequirement(
    ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const = 0;

  /**
   * Returns whether this ObjectCalcer supports moving.
   */
  virtual bool canMove() const;
  /**
   * Returns whether this ObjectCalcer can be translated at any position
   * in the coordinate plane.  Note that a ConstrainedPointType can be
   * moved, but cannot be moved everywhere.
   */
  virtual bool isFreelyTranslatable() const;
  /**
   * Moving an object most of the time signifies invoking changes in
   * some of its parents.  This method returns the set of parents that
   * will be changed in the move() method.  The object itself should
   * not be included.
   */
  virtual std::vector<ObjectCalcer*> movableParents() const;
  /**
   * In order to support simultaneously moving objects that are in
   * different locations, we need for each object a location that it
   * is assumed to be at, at the moment the moving starts.  This
   * method returns such a point.
   */
  virtual Coordinate moveReferencePoint() const;
  /**
   * This is the method that does the real moving work.  It will be
   * invoked to tell the object to move to the new location to.  After
   * this, the calc() method will be calced, so you only need to do
   * the real changes in this method, updating the ObjectImp should be
   * done in the calc() method, not here.
   */
  virtual void move( const Coordinate& to, const KigDocument& doc );

  /**
   * If this ObjectCalcer represents a curve, return true if the given
   * point is by construction on this curve.  If this ObjectCalcer
   * represents a point, return true if this point is by construction
   * on the given curve.
   */
  virtual bool isDefinedOnOrThrough( const ObjectCalcer* o ) const = 0;
};

/**
 * This is an ObjectCalcer that uses one of the various ObjectType's
 * to calculate its ObjectImp.  It basically forwards all of its
 * functionality to the ObjectType that it holds a pointer to.
 */
class ObjectTypeCalcer
  : public ObjectCalcer
{
  std::vector<ObjectCalcer*> mparents;
  const ObjectType* mtype;
  ObjectImp* mimp;
public:
  typedef myboost::intrusive_ptr<ObjectTypeCalcer> shared_ptr;
  /**
   * Construct a new ObjectTypeCalcer with a given type and parents.
   */
//  ObjectTypeCalcer( const ObjectType* type, const std::vector<ObjectCalcer*>& parents );
  /**
   * the sort boolean tells whether the sortArgs method should be invoked or not;
   * if not present
   */
  ObjectTypeCalcer( const ObjectType* type, const std::vector<ObjectCalcer*>& parents, bool sort=true );
  ~ObjectTypeCalcer();

  const ObjectImp* imp() const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> parents() const Q_DECL_OVERRIDE;
  void calc( const KigDocument& doc ) Q_DECL_OVERRIDE;

  /**
   * Set the parents of this ObjectTypeCalcer to np.  This object will
   * release the reference it had to its old parents, and take a new
   * one on the new parents.
   */
  void setParents( const std::vector<ObjectCalcer*> &np );
  void setType( const ObjectType* t );

  const ObjectType* type() const;

  const ObjectImpType* impRequirement(
    ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectCalcer* o ) const Q_DECL_OVERRIDE;
  bool canMove() const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable() const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents() const Q_DECL_OVERRIDE;
  Coordinate moveReferencePoint() const Q_DECL_OVERRIDE;
  void move( const Coordinate& to, const KigDocument& doc ) Q_DECL_OVERRIDE;
};

/**
 * This is an ObjectCalcer that keeps an ObjectImp, and never
 * calculates a new one.  It is a trivial, but very useful
 * ObjectCalcer.  It is used often in Kig, for holding data to be used
 * by other ObjectCalcer's.
 */
class ObjectConstCalcer
  : public ObjectCalcer
{
  ObjectImp* mimp;
public:
  typedef myboost::intrusive_ptr<ObjectConstCalcer> shared_ptr;

  /**
   * Construct a new ObjectConstCalcer with the given imp as the
   * stored ObjectImp.
   *
   * This class takes ownership of the imp you pass it, it should have
   * been constructed using new, and this class is responsible for
   * deleting it.
   */
  explicit ObjectConstCalcer( ObjectImp* imp );
  ~ObjectConstCalcer();

  const ObjectImp* imp() const Q_DECL_OVERRIDE;
  void calc( const KigDocument& doc ) Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> parents() const Q_DECL_OVERRIDE;

  /**
   * Set the ObjectImp of this ObjectConstCalcer to the given
   * newimp. The old one will be deleted.
   */
  void setImp( ObjectImp* newimp );
  /**
   * Set the ObjectImp of this ObjectConstCalcer to the given
   * newimp. The old one will not be deleted, but returned.
   */
  ObjectImp* switchImp( ObjectImp* newimp );

  const ObjectImpType* impRequirement(
    ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectCalcer* o ) const Q_DECL_OVERRIDE;
};

/**
 * This is an ObjectCalcer that has a single parent, and gets a
 * certain property from it in its calc() method.
 *
 * \see ObjectImp::property
 */
class ObjectPropertyCalcer
  : public ObjectCalcer
{
  ObjectImp* mimp;
  ObjectCalcer* mparent;
  int mpropgid;
/*
 * The following two variables are used for a caching
 * mechanism to avoid computing the Lid corresponding to
 * a Gid each time.  The "typeid" function in C++ is
 * used to compare the object class of the parent Imp;
 * the new Lid is computed only when the object class
 * of the parent imp changes.
 * As an alternative we could use "type" that each
 * object imp provides, but this is not completely
 * safe since in future extensions, in principle, two 
 * different ObjectImps could return the same "type" 
 * (although this is quite unlikely to happen)
 */
  mutable int mpropid;
//  mutable const ObjectImpType* mparenttype;
  mutable const std::type_info* mparenttype;
public:
  /**
   * Construct a new ObjectPropertyCalcer, that will get the property
   * from parent with number propid.
   */
  ObjectPropertyCalcer( ObjectCalcer* parent, int propid, bool islocal );
  ObjectPropertyCalcer( ObjectCalcer* parent, const char* pintname );
  ~ObjectPropertyCalcer();

  const ObjectImp* imp() const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> parents() const Q_DECL_OVERRIDE;
  void calc( const KigDocument& doc ) Q_DECL_OVERRIDE;

  ObjectCalcer* parent() const;

  const ObjectImpType* impRequirement(
    ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectCalcer* o ) const Q_DECL_OVERRIDE;

  int propLid() const;
  int propGid() const;
};

#endif
