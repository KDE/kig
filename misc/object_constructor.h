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

class KigPainter;
class KigDocument;
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

  // TODO
//   // return a
//   virtual QString useText( const Object& o,
//                            const Objects& sel
//     ) const = 0;

  // show a preliminary version of what you would do when handleArgs
  // would be called..  E.g. if this constructor normally constructs a
  // locus through some 5 points, then it will try to draw a locus
  // through whatever number of points it gets..
  virtual void handlePrelim( KigPainter& p,
                             const Objects& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const = 0;
};

class StandardConstructorBase
  : public ObjectConstructor
{
  const char* mdescname;
  const char* mdesc;
  const char* miconfile;
  const ArgParser margsparser;
public:
  StandardConstructorBase( const char* descname,
                           const char* desc,
                           const char* iconfile,
                           const ArgParser::spec* specs,
                           int n );

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
                           const Objects& parents,
                           const KigWidget& w ) const = 0;

  virtual Objects build(
    const Objects& os,
    KigDocument& d,
    KigWidget& w
    ) const = 0;
};

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
                   const Objects& parents,
                   const KigWidget& w ) const;

  Objects build( const Objects& os,
                 KigDocument& d,
                 KigWidget& w ) const;
};

#endif
