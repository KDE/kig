// object_hierarchy.h
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

#ifndef KIG_MISC_OBJECT_HIERARCHY_H
#define KIG_MISC_OBJECT_HIERARCHY_H

#include "../objects/common.h"

#include <map>
#include <vector>

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
  std::vector<const ObjectImpType*> margrequirements;

  // these two are really part of the constructor...
  int visit( const ObjectCalcer* o, std::map<const ObjectCalcer*, int>&,
             bool needed, bool neededatend = false);
  int storeObject( const ObjectCalcer*, const std::vector<ObjectCalcer*>& po, std::vector<int>& pl,
                   std::map<const ObjectCalcer*, int>& seenmap );

  friend bool operator==( const ObjectHierarchy& lhs, const ObjectHierarchy& rhs );

  void init( const std::vector<ObjectCalcer*>& from, const std::vector<ObjectCalcer*>& to );

public:
  ObjectHierarchy( const ObjectCalcer* from, const ObjectCalcer* to );
  ObjectHierarchy( const std::vector<ObjectCalcer*>& from, const ObjectCalcer* to );
  ObjectHierarchy( const std::vector<ObjectCalcer*>& from, const std::vector<ObjectCalcer*>& to );
  ObjectHierarchy( const ObjectHierarchy& h );
  ~ObjectHierarchy();

  // this creates a new ObjectHierarchy, that takes a.size() less
  // arguments, but uses copies of the ObjectImp's in a instead..
  ObjectHierarchy withFixedArgs( const Args& a ) const;

  std::vector<ObjectImp*> calc( const Args& a, const KigDocument& doc ) const;

  // saves the ObjectHierarchy data in children xml tags of parent..
  void serialize( QDomElement& parent, QDomDocument& doc ) const;
  // deserialize the ObjectHierarchy data from the xml element
  // parent..
  ObjectHierarchy( const QDomElement& parent );

  // build a set of objects that interdepend according to this
  // ObjectHierarchy..  Only the result objects are returned.  Helper
  // objects that connect the given objects with the returned objects,
  // can only be found by following the returned objects' parents()
  // methods..
  std::vector<ObjectCalcer*> buildObjects( const std::vector<ObjectCalcer*>& os, const KigDocument& ) const;

  ArgsParser argParser() const;

  uint numberOfArgs() const { return mnumberofargs; };
  uint numberOfResults() const { return mnumberofresults; };

  const ObjectImpType* idOfLastResult() const;
  bool resultDoesNotDependOnGiven() const;

  ObjectHierarchy transformFinalObject( const Transformation& t ) const;
};

bool operator==( const ObjectHierarchy& lhs, const ObjectHierarchy& rhs );

#endif
