// oldkigformat.h
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

#ifndef KIG_MISC_OLDKIGFORMAT_H
#define KIG_MISC_OLDKIGFORMAT_H

/**
 * This file contains some functions related to the parsing of
 * old-style ( kig 0.1 up to 0.3.1 ) kig files.  They are used by both
 * the import filter ( KigFilterNative::loadOld() in
 * filters/native/filter.cc ) and the macro import stuff (
 * MacroList::loadOld() in misc/lists.cc )..  The HierElem stuff is
 * used by the new native filter too..
 */

class QString;
class QCString;
class QDomElement;
class ReferenceObject;
class Objects;
class KigDocument;
class Object;

#include <vector>
#include <qdom.h>

/**
 * Lots of property names changed in the new kig format, especially
 * because we split up the translated property names and the internal
 * ones.  This function translates old names to new ones.
 */
QCString translateOldKigPropertyName( const QString& whichproperty );

/**
 * This function parses old ObjectHierarchy xml elements, and returns
 * a set of objects according to it.  firstelement is the first
 * element that needs parsing, you can handle some objects yourself
 * and then let this function handle the rest, by filling up
 * with the objects you want to use for the elements you skip.
 * This is necessary e.g. in the Macro importer, cause it needs to
 * handle the given objects itself..
 * final contains all objects in os that are marked final in the
 * hierarchy..
 * The objects are returned as the parents of retref, because we need
 * them to not be deleted..
 */
bool parseOldObjectHierarchyElements( const QDomElement& firstelement,
                                      const Objects& given,
                                      ReferenceObject& retref,
                                      Objects& final, const KigDocument& );

/**
 * This returns a random object for an old-style @param type.  It
 * makes sure that when oldElemToObject uses the generated object as a
 * parent for another object, it won't know the difference between the
 * object it's looking for, and the one we return here..
 * This is necessary for MacroList::loadOld(), which uses pseudo
 * objects for given objects of a old hierarchy, and then builds a
 * new hierarchy from the generated objects.
 */
Object* randomObjectForType( const QCString& type );

struct HierElem
{
  int id;
  std::vector<int> parents;
  QDomElement el;
};

void extendVect( std::vector<HierElem>& vect, uint size );

std::vector<HierElem> sortElems( const std::vector<HierElem> elems );

#endif
