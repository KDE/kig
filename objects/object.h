/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef OBJECT_H
#define OBJECT_H

#include <map>

#include <qcstring.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qnamespace.h>
#include <qcolor.h>

#include "../misc/objects.h"

class AbstractLine;
class Arc;
class Point;
class Segment;
class Vector;
class Ray;
class Line;
class Circle;
class Curve;
class NormalPoint;
class TextLabel;
class Types;
class KAction;
class KigDocument;
class KigWidget;
class KigMode;
class Type;
class KigPainter;
class Coordinate;
class Rect;
class NormalMode;
class ScreenInfo;
class StdConstructibleType;
class NormalModePopupObjects;

/**
 * Object is a base class representing all objects (e.g. points, lines etc.).
 *
 * @author Dominique Devriese
 */
class Object
  : protected Qt
{
private:
  static myvector<Type*> sbuiltintypes;
  static myvector<Type*> susertypes;
  static Types stypes;
public:
  // we have two types of types :)
  // 1 Builtin types: these are the C++ object types + perhaps in the
  //   future some macro types that we distribute along with Kig.
  // 2 User defined types: these are the macro's that the user
  //   defines, imports from a file, or those that are loaded on
  //   startup from a previous session ( if you define or load a new
  //   macro type, it is saved on exit, and loaded on startup ).

  // this contains _all_ types that are available.
  static const Types& types();

  // add a builtin type.  This is done only by the first part to be
  // started.  It does not notify the parts that a type has been
  // added, the part should take care of that itself.
  static void addBuiltinType( Type* );
  // add a user type:
  // This happens in two situations:
  // 1 on startup, the first part that is constructed loads user
  //   defined types from our config directory.  In this case,
  //   notifyParts is set to false, and the different parts aren't
  //   notified of the new type.
  // 2 some other time: either the user defines a new macro, or he
  //   loads one from disk, or ( maybe later :) he loads a plugin.  In
  //   this case, the different parts need to be notified of the
  //   change, and notifyParts == true...
  static void addUserType( Type*, bool notifyParts = true );
  static void addUserTypes( Types& t, bool notifyParts = true );

  static void removeUserType( Type* t );
//   static void removeUserTypes( Types& t, bool notifyParts = true );

  // these are static methods that should be considered constant..
  static const myvector<Type*>& builtinTypes();
  static const myvector<Type*>& userTypes();
  static Object* newObject( const QCString& type, const Objects& parents,
                            const std::map<QCString, QString>& params );
public:
  // if you want to use StdConstructionMode, you need a constructor
  // like Object( const Objects& args ); See the functions wantArgs(),
  // drawPrelim() and useText() for more information...
  Object();
  Object( const Object& o );
  virtual ~Object() {};

  /**
   * returns a copy of the object.  This should prolly simply do
   * "return new <Type>(*this);" (if you have a good copy constructor...)
   */
  virtual Object* copy() = 0;

  // only object types that have "parameters" need this, a parameter
  // is something which you cannot calculate from your parents,
  // e.g. an independent point's params are its coordinates, a
  // ConstrainedPoint's also got a param..
  // Objects that reimplement this should call Object::getParams() and
  // setParams() after you've handled your params.. --> to set params
  // common to all objects like the color...
  virtual std::map<QCString,QString> getParams ();
  virtual void setParams ( const std::map<QCString,QString>& );

  // getting types from this object: easier to type and supports
  // MacroObjectOne
  virtual Point* toPoint() { return 0; };
  virtual Segment* toSegment() { return 0; };
  virtual Vector* toVector() { return 0; };
  virtual Ray* toRay() { return 0; };
  virtual Line* toLine() { return 0; };
  virtual Circle* toCircle() { return 0; };
  virtual Curve* toCurve() { return 0; };
  virtual NormalPoint* toNormalPoint() { return 0; };
  virtual TextLabel* toTextLabel() { return 0; };
  virtual AbstractLine* toAbstractLine() { return 0; };
  virtual Arc* toArc() { return 0; };

  virtual const Point* toPoint() const { return 0; };
  virtual const Segment* toSegment() const { return 0; };
  virtual const Vector* toVector() const { return 0; };
  virtual const Ray* toRay() const { return 0; };
  virtual const Line* toLine() const { return 0; };
  virtual const Circle* toCircle() const { return 0; };
  virtual const Curve* toCurve() const { return 0; };
  virtual const NormalPoint* toNormalPoint() const { return 0; };
  virtual const TextLabel* toTextLabel() const { return 0; };
  virtual const AbstractLine* toAbstractLine() const { return 0; };
  virtual const Arc* toArc() const { return 0; };

  // type identification:
  // there are several functions: you don't need all of them in your
  // object type definition, but you do need a few:
  //
  // they come in pairs: a static and a virtual, the virtual should
  // always simply call the static.  The static should return
  // interesting stuff. the static func should be there, even though
  // it's not in this prototype, because we use template classes here
  // and there, and they expect the static funcs to be present
  //
  // the first two return the base type:  this should be something
  // like "point" for MidPoint, notice the minuscule.  Don't return
  // stuff like "midpoint".  Don't forget to put it between
  // "I18N_NOOP()", so it will be marked for translation
  virtual const QCString vBaseTypeName() const = 0;
  // static QCString sBaseTypeName();

  // this one is also used, it has no static counterpart, and you
  // shouldn't worry about this one
  const QString vTBaseTypeName() const;

  // this typename should be unique, e.g. for loading and saving. your
  // class also needs both of these, here's where you return something
  // like MidPoint, you don't need I18N_NOOP
  virtual const QCString vFullTypeName() const = 0;
  // static const QCString sFullTypeName();

  /**
   * Gives a descriptive name like i18n( "Circle defined by three
   * specified points" ) for CircleTTP... Do _NOT_ forget the i18n()
   * call !!!
   */
  virtual const QString vDescriptiveName() const = 0;
  // static const QString sDescriptiveName() const = 0;

  /**
   * Gives an explanation like i18n( "A circle which is defined by
   * its center and a point on its edge." ). Do _NOT_ forget the
   * i18n() call !!!
   */
  virtual const QString vDescription() const = 0;
  // static const QString sDescription() const = 0;

  /**
   * Gives the filename of a icon file.  e.g. "segment" for Segment,
   * "point4" for FixedPoint etc.  return 0 or "" if you don't have an
   * icon.
   */
  virtual const QCString vIconFileName() const = 0;
  // static const QCString vIconFileName() = 0;


  /**
   * This is used by TType, this implementation is good for almost
   * all, only NormalPoint and TextLabel need something else...
   */
  static KigMode* sConstructMode( StdConstructibleType* ourtype,
                                  KigDocument* theDoc,
                                  NormalMode* previousMode );

  // objects also need this static method:
  // static const char* sActionName();
  // this returns an internal name for the object.  This should be
  // something like objects_new_normalpoint...

  // drawing etc.
  // @p showSelect: whether selection should be shown (we don't want to
  // print selected stuff as selected...
  void drawWrap(KigPainter& p, bool ss) const;

  virtual void draw (KigPainter& p, bool showSelection) const = 0;

  /**
   * Whether the object contains o.
   * allowed_miss contains the maximum distance there may be between
   * o and your object...
   */
  virtual bool contains ( const Coordinate& o, const ScreenInfo& si ) const = 0;

  /**
   * Is this object in rect r ?
   */
  virtual bool inRect (const Rect& r) const = 0;

  // objects that want to use StdConstructingMode as a
  // constructingmode, need these 3 static functions...

  enum WantArgsResult { Complete, NotComplete, NotGood };
  // wantArgs asks whether the object can use the objects provided.
  // If the args are enough, return Complete, if you don't
  // want them return NotGood, and NotComplete otherwise...

  // static WantArgsResult sWantArgs( const Objects& );

  // this should return a text like i18n( "Circle through this point"
  // ) if you're a circle, and o is a point...  This is only
  // guaranteed to return something sane if sWantArgs(..) returns
  // (Not)Complete...
  // static QString sUseText( const Objects&, const Object* o )

  // this draws the object as if it had been constructed with os as
  // arguments..
  // static void sDrawPrelim( KigPainter&, const Objects& os );

  // for moving
  // sos contains the objects that are being moved.
  // the point is where the cursor is,  check the point implementation for an
  // example
  virtual void startMove( const Coordinate&, const ScreenInfo& ) = 0;
  virtual void moveTo(const Coordinate&) = 0;
  virtual void stopMove() = 0;

  /**
   * Informs the object that it ( or one of its parents ) has been
   * moved (or other situations), and that it should recalculate any
   * of its variables.  showingRect is the rect that is currently
   * showing.  Some objects need this ( e.g. Locus only wants points
   * that are in the rect, and throws away the rest... )
   */
  virtual void calc( const ScreenInfo& showingRect ) = 0;

  /**
   * This is a list of object-specific actions that appears when a
   * popup menu for a single object is shown.  An example is
   * NormalPoint that has a "redefine" action...
   */
  virtual void addActions( NormalModePopupObjects& );
  virtual void doNormalAction( int which, KigDocument* d, KigWidget* v, NormalMode* m );
  virtual void doPopupAction( int popupid, int actionid, KigDocument* d, KigWidget* w, NormalMode* m );

protected:
  QColor mColor;
public:
  QColor color() const { return mColor; };
  void setColor( const QColor c ) { mColor = c; };

 protected:
  // are we selected?
  bool selected;
 public:
  bool getSelected() const { return selected; };
  virtual void setSelected(bool in);
 private:
  // are we shown?
  bool mshown;
 public:
  bool shown() const;
  void setShown(bool s);
 protected:
  /**
   * An object is valid if it's in a state where its position and such
   * is defined: example: the intersection of a line and a circle:
   * this can be defined and undefined ( unless you start playing with
   * complex numbers, which is useless to us... )
   * objects are unvalid if
   * 1 its parents are unvalid..
   * 2 the object itself is unvalid due to its definition...
   * you should check for these cases in your calc() routine...
   */
  bool mvalid;
 public:
  bool valid() const;
 protected:
  /**
   * objects we know, and that know us: if they move, we move too, and vice versa
   */
  Objects children;

 public:
  const Objects& getChildren() const { return children;};
  /**
   * our children + our children's children + ...
   */
  Objects getAllChildren() const;

  void addChild(Object* o);
  void delChild(Object* o);
  /**
   * returns all objects the object depends upon ( the args it selected )
   */
  virtual Objects getParents() const = 0;
};

#endif // OBJECT_H
