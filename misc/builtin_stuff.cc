// builtin_stuff.cpp
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

#include "builtin_stuff.h"

#include "object_constructor.h"
#include "object_constructor_list.h"
#include "guiaction.h"
#include "guiaction_list.h"
#include "i18n.h"

#include "../objects/line_type.h"
#include "../objects/circle_type.h"
#include "../objects/conic_types.h"
#include "../objects/cubic_type.h"

void setupBuiltinStuff()
{
  static bool done = false;
  if ( ! done )
  {
    ObjectConstructorList* ctors = ObjectConstructorList::instance();
    GUIActionList* actions = GUIActionList::instance();
    ObjectConstructor* c = 0;

    // segment...
    c = new SimpleObjectTypeConstructor(
      SegmentABType::instance(), I18N_NOOP( "Segment" ),
      I18N_NOOP( "A segment constructed from its start and end point" ),
      "segment" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_segment" ) );

    // line by two points..
    c = new SimpleObjectTypeConstructor(
      LineABType::instance(), I18N_NOOP( "Line by Two Points" ),
      I18N_NOOP( "A line constructed through two points"), "line" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linettp" ) );

    // ray by two points..
    c = new SimpleObjectTypeConstructor(
      RayABType::instance(), I18N_NOOP( "Ray" ),
      I18N_NOOP( "A ray by its start point, and another point somewhere on it." ),
      "ray" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_ray" ) );

    // perpendicular line
    c = new SimpleObjectTypeConstructor(
      LinePerpendLPType::instance(), I18N_NOOP( "Perpendicular" ),
      I18N_NOOP( "A line constructed through a point, perpendicular on another line or segment." ),
      "perpendicular" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_lineperpend" ) );

    // parallel line
    c = new SimpleObjectTypeConstructor(
      LineParallelLPType::instance(), I18N_NOOP( "Parallel" ),
      I18N_NOOP( "A line constructed through a point, and parallel to another line or segment" ),
      "parallel" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_lineparallel" ) );

    // circle
    c = new SimpleObjectTypeConstructor(
      CircleBCPType::instance(), I18N_NOOP( "Circle by Center and Point" ),
      I18N_NOOP( "A circle constructed by its center and a point on its border" ),
      "circle" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_circlebcp" ) );

    c = new SimpleObjectTypeConstructor(
      CircleBTPType::instance(), I18N_NOOP( "Circle by Three Points" ),
      I18N_NOOP( "A circle constructed through three points" ),
      "circlebtp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_circlebtp" ) );

    c = new SimpleObjectTypeConstructor(
      ConicB5PType::instance(), I18N_NOOP( "Conic by Five Points" ),
      I18N_NOOP( "A conic constructed through five points" ),
      "conicb5p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicb5p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicBAAPType::instance(),
      I18N_NOOP( "Hyperbola by Asymptotes and Point" ),
      I18N_NOOP( "A hyperbola with given asymptotes through a point" ),
      "conicbaap" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicbaap" ) );

    c = new SimpleObjectTypeConstructor(
      EllipseBFFPType::instance(),
      I18N_NOOP( "Ellipse by Focuses and Point" ),
      I18N_NOOP( "An ellipse constructed by its focuses and a point on its border" ),
      "ellipsebffp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_ellipsebffp" ) );

    c = new SimpleObjectTypeConstructor(
      HyperbolaBFFPType::instance(),
      I18N_NOOP( "Hyperbola by Focuses and Point" ),
      I18N_NOOP( "A hyperbola constructed by its focuses and a point on its border" ),
      "hyperbolabffp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_hyperbolabffp" ) );

    c = new SimpleObjectTypeConstructor(
      ConicBDFPType::instance(),
      I18N_NOOP( "Conic by Directrix, Focus and Point" ),
      I18N_NOOP( "A conic with given directrix and focus, through a point" ),
      "conicbdfp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicbdfp" ) );

    c = new SimpleObjectTypeConstructor(
      ParabolaBTPType::instance(),
      I18N_NOOP( "Vertical Parabola by Three Points" ),
      I18N_NOOP( "A vertical parabola constructed through three points" ),
      "parabolabtp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_parabolabtp" ) );

    c = new SimpleObjectTypeConstructor(
      CubicB9PType::instance(),
      I18N_NOOP( "Cubic by Nine Points" ),
      I18N_NOOP( "A cubic constructed through nine points" ),
      "cubicb9p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_cubicb9p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicPolarPointType::instance(),
      I18N_NOOP( "Polar Point of a Line" ),
      I18N_NOOP( "The polar point of a line with respect to a conic." ),
      "polarpoint" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_pointpolar" ) );

    c = new SimpleObjectTypeConstructor(
      ConicPolarLineType::instance(),
      I18N_NOOP( "Polar Line of a Point" ),
      I18N_NOOP( "The polar line of a point with respect to a conic." ),
      "polarline" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linepolar" ) );

    c = new SimpleObjectTypeConstructor(
      CubicNodeB6PType::instance(),
      I18N_NOOP( "Cubic With Node by Six Points" ),
      I18N_NOOP( "A cubic with a nodal point at the origin through six points" ),
      "cubicnodeb6p" );
    // disabled cause it's only cluttering up the ui..
    // ctors->add( c );
    // actions->add( new ConstructibleAction( c, "objects_new_cubicnodeb6p" ) );

    actions->add( new ConstructPointAction( "objects_new_normalpoint" ) );
  };
  done = true;
};
