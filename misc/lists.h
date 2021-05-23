// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_LISTS_H
#define KIG_MISC_LISTS_H

#include <vector>
#include <set>

class GUIAction;
class ObjectConstructor;
class MacroConstructor;
class KigDocument;
class KigPart;
class KigWidget;
class QString;
class QDomElement;
class ObjectCalcer;

/**
 * List of GUIActions for the parts to show.  Note that the list owns
 * the actions it receives..
 */
class GUIActionList
{
public:
  typedef std::set<GUIAction*> avectype;
  typedef std::set<KigPart*> dvectype;
private:
  avectype mactions;
  dvectype mdocs;
  GUIActionList();
  ~GUIActionList();
public:
  static GUIActionList* instance();
  const avectype& actions() const { return mactions; }

  /**
   * register this document, so that it receives notifications for
   * added and removed actions.
   */
  void regDoc( KigPart* d );
  void unregDoc( KigPart* d );

  void add( GUIAction* a );
  void add( const std::vector<GUIAction*>& a );
  void remove( GUIAction* a );
  void remove( const std::vector<GUIAction*>& a );
};

/**
 * The list of object constructors for use in various places, e.g. RMB
 * menu's.  Note that the list owns the ctors it gets.
 */
class ObjectConstructorList
{
public:
  typedef std::vector<ObjectConstructor*> vectype;
private:
  vectype mctors;
  ObjectConstructorList();
  ~ObjectConstructorList();
public:
  static ObjectConstructorList* instance();
  void add( ObjectConstructor* a );
  void remove( ObjectConstructor* a );
  vectype ctorsThatWantArgs( const std::vector<ObjectCalcer*>&, const KigDocument&,
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
 * a simply equality operator for Macro class.
 */
bool operator==( const Macro& l, const Macro& r );

/**
 * This class keeps a list of all macro's, and allows them to be
 * easily accessed, added etc.  Macro objects are owned by this
 * list..
 */
class MacroList
{
public:
  typedef std::vector<Macro*> vectype;
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
   * Add a Macro \p m .  MacroList takes care of adding the action and
   * ctor in the relevant places.
   */
  void add( Macro* m );
  /**
   * Add the Macro's \p ms. MacroList takes care of adding the action
   * and ctor in the relevant places.
   */
  void add( const vectype& ms );

  /**
   * Remove macro \p m .  Macrolist takes care of deleting everything, and
   * unregistering the action and ctor from the relevant places.
   */
  void remove( Macro* m );

  /**
   * Save macro \p m to file \p f .
   */
  bool save( Macro* m, const QString& f );
  /**
   * Save macros \p ms to file \p f .
   */
  bool save( const vectype& ms, const QString& f );

  /**
   * load macro's from file \p f .
   * note that this just returns the loaded macro's, and doesn't add
   * them to the various lists.  Use add() if you want
   * that behaviour.
   * The fact that this functions requires a KigPart argument is
   * semantically incorrect, but i haven't been able to work around
   * it.
   */
  bool load( const QString& f, vectype& ret, const KigPart& );

  /**
   * get access to the list of macro's.
   */
  const vectype& macros() const;

private:
  bool loadNew( const QDomElement& docelem, std::vector<Macro*>& ret, const KigPart& );
};

#endif
