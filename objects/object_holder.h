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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_OBJECTS_OBJECT_HOLDER_H
#define KIG_OBJECTS_OBJECT_HOLDER_H

#include "object_calcer.h"

#include <qstring.h>

/**
 * An ObjectHolder represents an object as it is known to the
 * document.  It keeps a pointer to an ObjectCalcer, where it gets its
 * data ( the ObjectImp that the ObjectCalcer holds ) from.  It also
 * holds information about how to draw this ObjectImp on the window,
 * by keeping a pointer to an ObjectDrawer ( see below ).  In its draw
 * method, it gets the ObjectImp from the ObjectCalcer, and passes it
 * to the ObjectDrawer, asking it to draw the ObjectImp on the window.
 *
 * The document ( check the KigDocument class ) holds a list of these
 * ObjectHolder's.  This is its only link with the ObjectCalcer
 * dependency graph.
 *
 * An ObjectHolder keeps a reference to its * ObjectCalcer.
 */
class ObjectHolder
{
  ObjectCalcer::shared_ptr mcalcer;
  ObjectDrawer* mdrawer;
  ObjectConstCalcer::shared_ptr mnamecalcer;

public:
  /**
   * Construct a new ObjectHolder with a given ObjectCalcer and
   * ObjectDrawer, with a given name calcer.
   */
  ObjectHolder( ObjectCalcer* calcer, ObjectDrawer* drawer, ObjectConstCalcer* namecalcer );
  /**
   * Construct a new ObjectHolder with a given ObjectCalcer and
   * ObjectDrawer.
   */
  ObjectHolder( ObjectCalcer* calcer, ObjectDrawer* drawer );
  /**
   * equivalent to the previous constructor, but with a default
   * ObjectDrawer and no name.
   */
  ObjectHolder( ObjectCalcer* calcer );
  ~ObjectHolder();

  const ObjectImp* imp() const;
  const ObjectCalcer* calcer() const;
  ObjectCalcer* calcer();
  const ObjectDrawer* drawer() const;
  ObjectDrawer* drawer();
  // the following two return zero if no name is set.
  const ObjectConstCalcer* nameCalcer() const;
  ObjectConstCalcer* nameCalcer();
  /**
   * Setting the namecalcer is only allowed if previously none was
   * set. This way, we avoid keeping a useless namecalcer around if
   * no name is set.
   */
  void setNameCalcer( ObjectConstCalcer* namecalcer );

  /**
   * returns QString::null if no name is set.
   */
  const QString name() const;
  /**
   * Set the ObjectDrawer of this ObjectHolder to \p d , the old
   * ObjectDrawer is deleted.
   */
  void setDrawer( ObjectDrawer* d );
  /**
   * Set the ObjectDrawer of this ObjectHolder to \p d , the old
   * ObjectDrawer is not deleted, but returned.
   */
  ObjectDrawer* switchDrawer( ObjectDrawer* d );

  /**
   * Make our ObjectCalcer recalculate its ObjectImp.
   */
  void calc( const KigDocument& );
  /**
   * Draw this object on the given KigPainter.  If \p selected is true,
   * then it will be drawn in red, instead of its normal color.
   */
  void draw( KigPainter& p, bool selected ) const;
  /**
   * Returns whether this object contains the point \p p .
   */
  bool contains( const Coordinate& p, const KigWidget& w, bool nv = false ) const;
  /**
   * Returns whether this object is in the rectangle \p r .
   */
  bool inRect( const Rect& r, const KigWidget& w ) const;
  /**
   * Returns whether this object is shown.
   */
  bool shown() const;

  /**
   * This call is simply forwarded to the ObjectCalcer.  Check the
   * documentation of ObjectCalcer::moveReferencePoint() for more info.
   */
  const Coordinate moveReferencePoint() const;
  /**
   * This call is simply forwarded to the ObjectCalcer.  Check the
   * documentation of ObjectCalcer::move() for more info.
   */
  void move( const Coordinate& to, const KigDocument& );
  /**
   * This call is simply forwarded to the ObjectCalcer.  Check the
   * documentation of ObjectCalcer::canMove() for more info.
   */
  bool canMove() const;
  /**
   * This call is simply forwarded to the ObjectCalcer.  Check the
   * documentation of ObjectCalcer::isFreelyTranslatable() for more info.
   */
  bool isFreelyTranslatable() const;

  /**
   * Return a statement saying something like "select this segment" or
   * "select segment ab" depending on whether this ObjectHolder has a
   * name.
   */
  QString selectStatement() const;
};

#endif
