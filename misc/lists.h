// lists.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_LISTS_H
#define KIG_MISC_LISTS_H

#include "objects.h"

class GUIAction;
class ObjectConstructor;
class MacroConstructor;
class Objects;
class KigDocument;
class KigWidget;
class QString;
class QDomElement;

/**
 * List of GUIActions for the parts to show.  Note that the list owns
 * the actions it receives..
 */
class GUIActionList
{
public:
  typedef myvector<GUIAction*> avectype;
  typedef myvector<KigDocument*> dvectype;
private:
  avectype mactions;
  dvectype mdocs;
  GUIActionList();
  ~GUIActionList();
public:
  static GUIActionList* instance();
  const avectype& actions() const { return mactions; };

  // register this document, so that it receives notifications for
  // added and removed actions..
  void regDoc( KigDocument* d );
  void unregDoc( KigDocument* d );

  void add( GUIAction* a );
  void add( const myvector<GUIAction*>& a );
  void remove( GUIAction* a );
  void remove( const myvector<GUIAction*>& a );
};

/**
 * The list of object constructors for use in various places, e.g. RMB
 * menu's.  Note that the list owns the ctors it gets..
 */
class ObjectConstructorList
{
public:
  typedef myvector<ObjectConstructor*> vectype;
private:
  vectype mctors;
  ObjectConstructorList();
  ~ObjectConstructorList();
public:
  static ObjectConstructorList* instance();
  void add( ObjectConstructor* a );
  void remove( ObjectConstructor* a );
  vectype ctorsThatWantArgs( const Objects&, const KigDocument&,
                             const KigWidget&, bool completeOnly = false
    ) const;
  const vectype& constructors() const;
};

/**
 * this is just a simple data struct.  doesn't have any functionality
 * of its own..
 */
class Macro
{
public:
  GUIAction* action;
  MacroConstructor* ctor;
  Macro( GUIAction* a, MacroConstructor* c );
  ~Macro();
};

/**
 * This class keeps a list of all macro's, and allows them to be
 * easily accessed, added etc.  Macro objects are owned by this
 * list..
 */
class MacroList
{
public:
  typedef myvector<Macro*> vectype;
private:
  vectype mdata;
  MacroList();
  ~MacroList();
public:
  /**
   * MacroList is a singleton
   */
  static MacroList* instance();

  /**
   * Add a Macro m.  MacroList takes care of adding the action and
   * ctor in the relevant places..
   */
  void add( Macro* m );
  void add( const vectype& ms );

  /**
   * Remove macro m.  Macrolist takes care of deleting everything, and
   * unregistering the action and ctor from the relevant places..
   */
  void remove( Macro* m );

  /**
   * Save macro(s) m to file f..
   */
  bool save( Macro* m, const QString& f );
  bool save( const vectype& ms, const QString& f );

  /**
   * load macro's from file f..
   * note that this just returns the loaded macro's, and doesn't add
   * them to the various lists.  Use add() if you want
   * that behaviour..
   */
  bool load( const QString& f, vectype& ret );

  /**
   * get access to the list of macro's..
   */
  const vectype& macros() const;

private:
  bool loadNew( const QDomElement& docelem, myvector<Macro*>& ret );
  bool loadOld( const QDomElement& docelem, myvector<Macro*>& ret );
};

#endif
