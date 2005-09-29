// Copyright (C) 2002-2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_OBJECT_CONSTRUCTOR_H
#define KIG_MISC_OBJECT_CONSTRUCTOR_H

#include "argsparser.h"
#include "object_hierarchy.h"

class KigPainter;
class KigDocument;
class KigGUIAction;
class KigWidget;
class ArgsParserObjectType;
class ObjectType;
class BaseConstructMode;

class QString;
class QCString;

/**
 * This class represents a way to construct a set of objects from a
 * set of other objects.  There are some important child classes, like
 * MacroConstructor, StandardObjectConstructor etc. ( see below )
 * Actually, it is more generic than that, it provides a way to do
 * _something_ with a set of objects, but for now, i only use it to
 * construct objects.  Maybe some day, i'll find something more
 * interesting to do with it, who knows... ;)
 */
class ObjectConstructor
{
public:
  virtual ~ObjectConstructor();

  virtual const QString descriptiveName() const = 0;
  virtual const QString description() const = 0;
  virtual const QCString iconFileName( const bool canBeNull = false ) const = 0;

  /**
   * the following function is called in case of duplication of arguments
   * and returns true if this is acceptable; this will return false for
   * typical objects
   */
  virtual const bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, 
                              const int& ) const = 0;
  /**
   * can this constructor do something useful with \p os ?  return
   * ArgsParser::Complete, Valid or NotGood
   */
  virtual const int wantArgs( const std::vector<ObjectCalcer*>& os,
                              const KigDocument& d,
                              const KigWidget& v
    ) const = 0;

  /**
   * do something fun with \p os .. This func is only called if wantArgs
   * returned Complete.. handleArgs should <i>not</i> do any
   * drawing.. after somebody calls this function, he should
   * redrawScreen() himself..
   */
  virtual void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const = 0;

  /**
   * return a string describing what you would use \p o for if it were
   * selected...  \p o should be part of \p sel .
   */
  virtual QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const = 0;

  /**
   * return a string describing what argument you want next, if the
   * given selection of objects were selected.
   */
  virtual QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const = 0;

  /**
   * show a preliminary version of what you would do when \ref handleArgs
   * would be called..  E.g. if this constructor normally constructs a
   * locus through some 5 points, then it will try to draw a locus
   * through whatever number of points it gets..
   */
  virtual void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const = 0;

  virtual void plug( KigPart* doc, KigGUIAction* kact ) = 0;

  virtual bool isTransform() const = 0;
  virtual bool isTest() const;
  virtual bool isIntersection() const;

  /**
   * Which construct mode should be used for this ObjectConstructor.
   * In fact, this is not a pretty design.  The Kig
   * GUIAction-ObjectConstructor stuff should be reworked into a
   * general GUIAction, which just models something which can be
   * executed given a certain number of arguments.  The code for
   * drawPrelim and such should all be in the ConstructMode, and the
   * new GUIAction should just start the correct KigMode with the
   * correct arguments.
   *
   * This function is only overridden in TestConstructor.
   */
  virtual BaseConstructMode* constructMode( KigPart& doc );
};

/**
 * This class provides wraps ObjectConstructor in a more simple
 * interface for the most common object types..
 */
class StandardConstructorBase
  : public ObjectConstructor
{
  const char* mdescname;
  const char* mdesc;
  const char* miconfile;
  const ArgsParser& margsparser;
public:
  StandardConstructorBase( const char* descname,
                           const char* desc,
                           const char* iconfile,
                           const ArgsParser& parser );

  virtual ~StandardConstructorBase();

  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName( const bool canBeNull = false ) const;

  const bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, 
                                  const int& ) const;
  virtual const int wantArgs(
    const std::vector<ObjectCalcer*>& os, const KigDocument& d,
    const KigWidget& v
    ) const;

  void handleArgs( const std::vector<ObjectCalcer*>& os,
                   KigPart& d,
                   KigWidget& v
    ) const;

  void handlePrelim( KigPainter& p, const std::vector<ObjectCalcer*>& sel,
                     const KigDocument& d, const KigWidget& v
    ) const;

  virtual void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                           const KigDocument& ) const = 0;

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                   const KigDocument& d, const KigWidget& v ) const;

  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;

  virtual std::vector<ObjectHolder*> build(
    const std::vector<ObjectCalcer*>& os,
    KigDocument& d, KigWidget& w
    ) const = 0;
};

/**
 * A standard implementation of StandardConstructorBase for simple
 * types..
 */
class SimpleObjectTypeConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
public:
  SimpleObjectTypeConstructor(
    const ArgsParserObjectType* t, const char* descname,
    const char* desc, const char* iconfile );

  ~SimpleObjectTypeConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;

  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os,
                                    KigDocument& d,
                                    KigWidget& w ) const;

  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

/**
 * A standard implementation of StandardConstructorBase for property
 * objects...
 */
class PropertyObjectConstructor
  : public StandardConstructorBase
{
  ArgsParser mparser;
  const char* mpropinternalname;
public:
  PropertyObjectConstructor(
    const ObjectImpType* imprequirement, const char* usetext,
    const char* selectstat, const char* descname, const char* desc,
    const char* iconfile, const char* propertyinternalname );

  ~PropertyObjectConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;

  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os,
                                    KigDocument& d, KigWidget& w ) const;

  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

/**
 * This class is the equivalent of \ref SimpleObjectTypeConstructor
 * for object types that are constructed in groups of more than one.
 * For example, the intersection of a circle and line in general
 * produces two points, in general.  Internally, we differentiate
 * betweem them by passing them a parameter of ( in this case ) 1 or
 * -1.  There are still other object types that work the same, and
 * they all require this sort of parameter.
 * E.g. CubicLineIntersectionType takes a parameter between 1 and 3.
 * This class knows about that, and constructs the objects along this
 * scheme..
 */
class MultiObjectTypeConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  std::vector<int> mparams;
  ArgsParser mparser;
public:
  MultiObjectTypeConstructor(
    const ArgsParserObjectType* t, const char* descname,
    const char* desc, const char* iconfile,
    const std::vector<int>& params );
  MultiObjectTypeConstructor(
    const ArgsParserObjectType* t, const char* descname,
    const char* desc, const char* iconfile,
    int a, int b, int c = -999, int d = -999 );
  ~MultiObjectTypeConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;

  std::vector<ObjectHolder*> build(
    const std::vector<ObjectCalcer*>& os,
    KigDocument& d, KigWidget& w ) const;

  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

/**
 * This class is a collection of some other ObjectConstructors, that
 * makes them appear to the user as a single ObjectConstructor.  It is
 * e.g. used for the "intersection" constructor.
 */
class MergeObjectConstructor
  : public ObjectConstructor
{
  const char* mdescname;
  const char* mdesc;
  const char* miconfilename;
  typedef std::vector<ObjectConstructor*> vectype;
  vectype mctors;
public:
  MergeObjectConstructor( const char* descname, const char* desc,
                          const char* iconfilename );
  ~MergeObjectConstructor();

  void merge( ObjectConstructor* e );

  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName( const bool canBeNull = false ) const;

  const bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, 
                                  const int& ) const;
  const int wantArgs( const std::vector<ObjectCalcer*>& os,
                      const KigDocument& d,
                      const KigWidget& v
    ) const;

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                   const KigDocument& d, const KigWidget& v ) const;

  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;

  void handleArgs( const std::vector<ObjectCalcer*>& os, KigPart& d, KigWidget& v ) const;

  void handlePrelim( KigPainter& p, const std::vector<ObjectCalcer*>& sel,
                     const KigDocument& d, const KigWidget& v ) const;

  void plug( KigPart* doc, KigGUIAction* kact );

  bool isTransform() const;
};

/**
 * MacroConstructor is a class that represents Kig macro's: these are
 * constructed by the user, and defined by a set of input and a set of
 * output objects.  The macro-constructor saves the way in which the
 * output objects have been built from the input objects, and when
 * given similar input objects, it will produce objects in the given
 * way.  The data is saved in a \ref ObjectHierarchy.
 */
class MacroConstructor
  : public ObjectConstructor
{
  ObjectHierarchy mhier;
  QString mname;
  QString mdesc;
  bool mbuiltin;
  QCString miconfile;
  ArgsParser mparser;
public:
  MacroConstructor( const std::vector<ObjectCalcer*>& input, const std::vector<ObjectCalcer*>& output,
                    const QString& name, const QString& description,
                    const QCString& iconfile = 0 );
  MacroConstructor( const ObjectHierarchy& hier, const QString& name,
                    const QString& desc,
                    const QCString& iconfile = 0 );
  ~MacroConstructor();

  const ObjectHierarchy& hierarchy() const;

  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName( const bool canBeNull = false ) const;

  const bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, 
                                  const int& ) const;
  const int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d,
                      const KigWidget& v ) const;

  void handleArgs( const std::vector<ObjectCalcer*>& os, KigPart& d,
                   KigWidget& v ) const;

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                   const KigDocument& d, const KigWidget& v
    ) const;

  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const;

  void handlePrelim( KigPainter& p, const std::vector<ObjectCalcer*>& sel,
                     const KigDocument& d, const KigWidget& v
    ) const;

  void plug( KigPart* doc, KigGUIAction* kact );

  void setBuiltin( bool builtin );

  /**
   * is this the ctor for a transformation type.  We want to know this
   * cause transform types are shown separately in an object's RMB
   * menu..
   */
  bool isTransform() const;

  void setName( const QString& name );
  void setDescription( const QString& desc );
  void setIcon( QCString& icon );
};

#endif
