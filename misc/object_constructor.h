// object_constructor.h
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_MISC_OBJECT_CONSTRUCTOR_H
#define KIG_MISC_OBJECT_CONSTRUCTOR_H

#include "argsparser.h"
#include "object_hierarchy.h"

class KigPainter;
class KigDocument;
class KigGUIAction;
class KigWidget;
class ObjectType;
class Objects;
class Object;

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
  virtual const QCString iconFileName() const = 0;

  // can this constructor do something useful with os ?  return
  // ArgsChecker::Complete, Valid or NotGood
  virtual const int wantArgs( const Objects& os,
                              const KigDocument& d,
                              const KigWidget& v
    ) const = 0;

  // do something fun with os..  this func is only called if wantArgs
  // returned Complete.. handleArgs should _not_ do any
  // drawing.. after somebody calls this function, he should
  // redrawScreen() himself..
  virtual void handleArgs( const Objects& os,
                           KigDocument& d,
                           KigWidget& v
    ) const = 0;

  // return a string describing what you would use o for if it were
  // selected...
  virtual QString useText( const Object& o, const Objects& sel,
                           const KigDocument& d, const KigWidget& v
    ) const = 0;

  // show a preliminary version of what you would do when handleArgs
  // would be called..  E.g. if this constructor normally constructs a
  // locus through some 5 points, then it will try to draw a locus
  // through whatever number of points it gets..
  virtual void handlePrelim( KigPainter& p,
                             const Objects& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const = 0;

  virtual void plug( KigDocument* doc, KigGUIAction* kact ) = 0;
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
  const ArgParser& margsparser;
public:
  StandardConstructorBase( const char* descname,
                           const char* desc,
                           const char* iconfile,
                           const ArgParser& parser );

  virtual ~StandardConstructorBase();

  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;

  const int wantArgs(
    const Objects& os, const KigDocument& d,
    const KigWidget& v
    ) const;

  void handleArgs( const Objects& os,
                   KigDocument& d,
                   KigWidget& v
    ) const;

  void handlePrelim( KigPainter& p, const Objects& sel,
                     const KigDocument& d, const KigWidget& v
    ) const;

  virtual void drawprelim( KigPainter& p,
                           const Objects& parents ) const = 0;

  QString useText( const Object& o, const Objects& sel, const KigDocument& d,
                   const KigWidget& v ) const;

  virtual Objects build(
    const Objects& os,
    KigDocument& d,
    KigWidget& w
    ) const = 0;
};

/**
 * A standard implementation of StandardConstructorBase for simple
 * types..
 */
class SimpleObjectTypeConstructor
  : public StandardConstructorBase
{
  const ObjectType* mtype;
public:
  SimpleObjectTypeConstructor(
    const ObjectType* t, const char* descname,
    const char* desc, const char* iconfile );

  ~SimpleObjectTypeConstructor();

  void drawprelim( KigPainter& p,
                   const Objects& parents ) const;

  Objects build( const Objects& os,
                 KigDocument& d,
                 KigWidget& w ) const;

  void plug( KigDocument* doc, KigGUIAction* kact );
};

/**
 * This class is the equivalent of @ref SimpleObjectTypeConstructor
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
  const ObjectType* mtype;
  std::vector<int> mparams;
  ArgParser mparser;
public:
  MultiObjectTypeConstructor(
    const ObjectType* t, const char* descname,
    const char* desc, const char* iconfile,
    const std::vector<int>& params );
  MultiObjectTypeConstructor(
    const ObjectType* t, const char* descname,
    const char* desc, const char* iconfile,
    int a, int b, int c = -999, int d = -999 );
  ~MultiObjectTypeConstructor();

  void drawprelim( KigPainter& p,
                   const Objects& parents ) const;

  Objects build( const Objects& os,
                 KigDocument& d,
                 KigWidget& w ) const;

  void plug( KigDocument* doc, KigGUIAction* kact );
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
  const QCString iconFileName() const;

  const int wantArgs( const Objects& os,
                      const KigDocument& d,
                      const KigWidget& v
    ) const;

  QString useText( const Object& o, const Objects& sel, const KigDocument& d,
                   const KigWidget& v ) const;

  void handleArgs( const Objects& os, KigDocument& d, KigWidget& v ) const;

  void handlePrelim( KigPainter& p, const Objects& sel,
                     const KigDocument& d, const KigWidget& v ) const;

  void plug( KigDocument* doc, KigGUIAction* kact );
};

class MacroConstructor
  : public ObjectConstructor
{
  ObjectHierarchy mhier;
  const QString mname;
  const QString mdesc;
  ArgParser mparser;
public:
  MacroConstructor( const Objects& input, const Objects& output,
                    const QString name, const QString description );
  ~MacroConstructor();

  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;

  // can this constructor do something useful with os ?  return
  // ArgsChecker::Complete, Valid or NotGood
  const int wantArgs( const Objects& os, const KigDocument& d,
                      const KigWidget& v ) const;

  // do something fun with os..  this func is only called if wantArgs
  // returned Complete.. handleArgs should _not_ do any
  // drawing.. after somebody calls this function, he should
  // redrawScreen() himself..
  void handleArgs( const Objects& os, KigDocument& d,
                   KigWidget& v ) const;

  // return a string describing what you would use o for if it were
  // selected...
  QString useText( const Object& o, const Objects& sel,
                   const KigDocument& d, const KigWidget& v
    ) const;

  // show a preliminary version of what you would do when handleArgs
  // would be called..  E.g. if this constructor normally constructs a
  // locus through some 5 points, then it will try to draw a locus
  // through whatever number of points it gets..
  void handlePrelim( KigPainter& p, const Objects& sel,
                     const KigDocument& d, const KigWidget& v
    ) const;

  void plug( KigDocument* doc, KigGUIAction* kact );
};

#endif
