// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_CALCPATHS_H
#define KIG_MISC_CALCPATHS_H

#include "../objects/common.h"

/**
 * This function sorts \p os such that they're in the right order for
 * calc()-ing.  This means that child objects must appear after their
 * parents ( for you graph people, this is just a topological sort.. )
 */
std::vector<ObjectCalcer*> calcPath( const std::vector<ObjectCalcer*>& os );

/**
 * This is a different function for more or less the same purpose.  It
 * takes a few Objects, which are considered to have been calced
 * already.  Then, it puts the necessary part of their children in the
 * right order, so that calc()-ing correctly updates all of their data
 * ( they're calc'ed in the right order, i mean... ).  The objects in
 * \p from are normally not included in the output, unless they appear
 * somewhere in the middle of the calc-path towards \p to ...
 */
std::vector<ObjectCalcer*> calcPath( const std::vector<ObjectCalcer*>& from, const ObjectCalcer* to );

/**
 * This function returns all objects on the side of the path through
 * the dependency tree from \p from down to \p to . This means that we
 * look for any objects that don't depend on any of the objects in
 * \p from themselves, but of which one of the direct children does.
 * We need this function for Locus stuff...
 */
std::vector<ObjectCalcer*> sideOfTreePath( const std::vector<ObjectCalcer*>& from, const ObjectCalcer* to );

/**
 * This function returns all objects above the given in the
 * dependency graph.  The given objects \p objs are also included
 * themselves.
 */
std::vector<ObjectCalcer*> getAllParents( const std::vector<ObjectCalcer*>& objs );
/**
 * \overload
 */
std::vector<ObjectCalcer*> getAllParents( ObjectCalcer* obj );

/**
 * This function returns all objects below the objects in \p objs in the
 * dependency graphy.  The objects in \p objs are also included
 * themselves.
 */
std::set<ObjectCalcer*> getAllChildren( const std::vector<ObjectCalcer*> &objs );

/**
 * \overload
 */
std::set<ObjectCalcer*> getAllChildren( ObjectCalcer* obj );

/**
 * Returns true if \p o is a descendant of any of the objects in \p os .
 */
bool isChild( const ObjectCalcer* o, const std::vector<ObjectCalcer*>& os );
bool isChild( const ObjectCalcer* o, ObjectCalcer* op );

/**
 * Return true if the given \p point is ( by construction ) on the given
 * \p curve.  This means that it is either a constrained point on the
 * curve, or the curve is constructed through the point, or the point
 * is an intersection point of the curve with another curve.
 * Note that it is assumed that the given point is in fact a point and the
 * given curve is in fact a curve.
 */
bool isPointOnCurve( const ObjectCalcer* point, const ObjectCalcer* curve );

#endif
