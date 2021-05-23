// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_OBJECT_HIERARCHY_H
#define KIG_MISC_OBJECT_HIERARCHY_H

#include "../objects/common.h"

#include <map>
#include <vector>
#include <string>

class ObjectImpType;
class ArgsParser;

class ObjectHierarchy
{
public:
  class Node;
private:
  std::vector<Node*> mnodes;
  uint mnumberofargs;
  uint mnumberofresults;
  bool msaveinputtags;   // if true the UseText and SelectStatement are serialized for saving
  std::vector<const ObjectImpType*> margrequirements;
  std::vector<std::string> musetexts;
  std::vector<std::string> mselectstatements;

  // these two are really part of the constructor...
  int visit( const ObjectCalcer* o, std::map<const ObjectCalcer*, int>&,
             bool needed, bool neededatend = false);
  int storeObject( const ObjectCalcer*, const std::vector<ObjectCalcer*>& po, std::vector<int>& pl,
                   std::map<const ObjectCalcer*, int>& seenmap );

  friend bool operator==( const ObjectHierarchy& lhs, const ObjectHierarchy& rhs );

  void init( const std::vector<ObjectCalcer*>& from, const std::vector<ObjectCalcer*>& to );

  /**
   * this constructor is private since it should be used only by the static
   * constructor buildSafeObjectHierarchy
   *
   * \see ObjectHierarchy::buildSafeObjectHierarchy
   */
  ObjectHierarchy();

public:
  ObjectHierarchy( const ObjectCalcer* from, const ObjectCalcer* to );
  ObjectHierarchy( const std::vector<ObjectCalcer*>& from, const ObjectCalcer* to );
  ObjectHierarchy( const std::vector<ObjectCalcer*>& from, const std::vector<ObjectCalcer*>& to );
  ObjectHierarchy( const ObjectHierarchy& h );
  ~ObjectHierarchy();

  // The default provided version is wrong, since we don't use it just mark it as deleted, if you need it, implement it following the copy constructor
  ObjectHierarchy &operator=( const ObjectHierarchy& h ) = delete; 

  /**
   * this creates a new ObjectHierarchy, that takes a.size() less
   * arguments, but uses copies of the ObjectImp's in \p a instead.
   */
  ObjectHierarchy withFixedArgs( const Args& a ) const;

  std::vector<ObjectImp*> calc( const Args& a, const KigDocument& doc ) const;

  /**
   * saves the ObjectHierarchy data in children xml tags of \p parent .
   */
  void serialize( QDomElement& parent, QDomDocument& doc ) const;
  /**
   * Deserialize the ObjectHierarchy data from the xml element \p parent .
   * Since this operation can fail for some reasons, we provide it as a
   * static to return 0 in case of error.
   */
  static ObjectHierarchy* buildSafeObjectHierarchy( const QDomElement& parent, QString& error );
//  explicit ObjectHierarchy( const QDomElement& parent );

  /**
   * build a set of objects that interdepend according to this
   * ObjectHierarchy..  Only the result objects are returned.  Helper
   * objects that connect the given objects with the returned objects,
   * can only be found by following the returned objects' parents()
   * methods..
   */
  std::vector<ObjectCalcer*> buildObjects( const std::vector<ObjectCalcer*>& os, const KigDocument& ) const;

  ArgsParser argParser() const;

  uint numberOfArgs() const { return mnumberofargs; }
  uint numberOfResults() const { return mnumberofresults; }

  const ObjectImpType* idOfLastResult() const;

  bool resultDependsOnGiven() const;
  bool allGivenObjectsUsed() const;

  ObjectHierarchy transformFinalObject( const Transformation& t ) const;
};

bool operator==( const ObjectHierarchy& lhs, const ObjectHierarchy& rhs );

#endif
