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
#include "lists.h"
#include "special_constructors.h"
#include "guiaction.h"
#include "i18n.h"

#include "../objects/line_type.h"
#include "../objects/intersection_types.h"
#include "../objects/circle_type.h"
#include "../objects/conic_types.h"
#include "../objects/cubic_type.h"
#include "../objects/other_type.h"
#include "../objects/transform_types.h"
#include "../objects/point_type.h"

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
      LineABType::instance(), I18N_NOOP( "Line by two points" ),
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
      CircleBCPType::instance(), I18N_NOOP( "Circle by center and point" ),
      I18N_NOOP( "A circle constructed by its center and a point on its border" ),
      "circlebcp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_circlebcp" ) );

    c = new SimpleObjectTypeConstructor(
      CircleBTPType::instance(), I18N_NOOP( "Circle by three points" ),
      I18N_NOOP( "A circle constructed through three points" ),
      "circlebtp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_circlebtp" ) );

    c = new SimpleObjectTypeConstructor(
      ConicB5PType::instance(), I18N_NOOP( "Conic by five points" ),
      I18N_NOOP( "A conic constructed through five points" ),
      "conicb5p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicb5p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicBAAPType::instance(),
      I18N_NOOP( "Hyperbola by asymptotes and point" ),
      I18N_NOOP( "A hyperbola with given asymptotes through a point" ),
      "conicbaap" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicbaap" ) );

    c = new SimpleObjectTypeConstructor(
      EllipseBFFPType::instance(),
      I18N_NOOP( "Ellipse by focuses and point" ),
      I18N_NOOP( "An ellipse constructed by its focuses and a point on its border" ),
      "ellipsebffp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_ellipsebffp" ) );

    c = new SimpleObjectTypeConstructor(
      HyperbolaBFFPType::instance(),
      I18N_NOOP( "Hyperbola by focuses and point" ),
      I18N_NOOP( "A hyperbola constructed by its focuses and a point on its border" ),
      "hyperbolabffp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_hyperbolabffp" ) );

    c = new SimpleObjectTypeConstructor(
      ConicBDFPType::instance(),
      I18N_NOOP( "Conic by directrix, focus and point" ),
      I18N_NOOP( "A conic with given directrix and focus, through a point" ),
      "conicbdfp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicbdfp" ) );

    c = new SimpleObjectTypeConstructor(
      ParabolaBTPType::instance(),
      I18N_NOOP( "Vertical parabola by three points" ),
      I18N_NOOP( "A vertical parabola constructed through three points" ),
      "parabolabtp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_parabolabtp" ) );

    c = new SimpleObjectTypeConstructor(
      CubicB9PType::instance(),
      I18N_NOOP( "Cubic by nine points" ),
      I18N_NOOP( "A cubic constructed through nine points" ),
      "cubicb9p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_cubicb9p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicPolarPointType::instance(),
      I18N_NOOP( "Polar point of a line" ),
      I18N_NOOP( "The polar point of a line with respect to a conic." ),
      "polarpoint" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_pointpolar" ) );

    c = new SimpleObjectTypeConstructor(
      ConicPolarLineType::instance(),
      I18N_NOOP( "Polar line of a point" ),
      I18N_NOOP( "The polar line of a point with respect to a conic." ),
      "polarline" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linepolar" ) );

    c = new SimpleObjectTypeConstructor(
      CubicNodeB6PType::instance(),
      I18N_NOOP( "Cubic with node by six points" ),
      I18N_NOOP( "A cubic with a nodal point at the origin through six points" ),
      "cubicnodeb6p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_cubicnodeb6p" ) );

    c = new SimpleObjectTypeConstructor(
      CubicCuspB4PType::instance(),
      I18N_NOOP( "Cubic with cusp by four points" ),
      I18N_NOOP( "A cubic with a horizontal cusp at the origin through four points" ),
      "cubiccuspb4p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_cubiccuspb4p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicDirectrixType::instance(),
      I18N_NOOP( "Directrix of a conic" ),
      I18N_NOOP( "The directrix line of a conic." ),
      "directrix" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linedirectrix" ) );

    c = new SimpleObjectTypeConstructor(
      AngleType::instance(),
      I18N_NOOP( "Angle by three points" ),
      I18N_NOOP( "An angle defined by three points" ),
      "angle" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_angle" ) );

    c = new SimpleObjectTypeConstructor(
      EquilateralHyperbolaB4PType::instance(),
      I18N_NOOP( "Equilateral hyperbola by four points" ),
      I18N_NOOP( "An equilateral hyperbola constructed through four points" ),
      "equilateralhyperbolab4p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_equilateralhyperbolab4p" ) );

    c = new SimpleObjectTypeConstructor(
      MidPointType::instance(),
      I18N_NOOP( "Midpoint" ),
      I18N_NOOP( "The midpoint of a segment or two other points" ),
      "bisection" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_midpoint" ) );

    c = new SimpleObjectTypeConstructor(
      VectorType::instance(),
      I18N_NOOP( "Vector" ),
      I18N_NOOP( "Construct a vector from two given points." ),
      "vector" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_vector" ) );

    c = new SimpleObjectTypeConstructor(
      ParabolaBDPType::instance(),
      I18N_NOOP( "Parabola by directrix and focus" ),
      I18N_NOOP( "A parabola defined by its directrix and focus" ),
      "parabolabdp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_parabolabdp" ) );

    // Transformation stuff..
    c = new SimpleObjectTypeConstructor(
      TranslatedType::instance(),
      I18N_NOOP( "Translate" ),
      I18N_NOOP( "The translation of an object by a vector" ),
      "translation" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_translation" ) );

    c = new SimpleObjectTypeConstructor(
      PointReflectionType::instance(),
      I18N_NOOP( "Reflect over point" ),
      I18N_NOOP( "The reflection of an object over a point" ),
      "centralsymmetry" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_pointreflection" ) );

    c = new SimpleObjectTypeConstructor(
      LineReflectionType::instance(),
      I18N_NOOP( "Mirror" ),
      I18N_NOOP( "The reflection of an object over a line" ),
      "mirrorpoint" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linereflection" ) );

    c = new SimpleObjectTypeConstructor(
      RotationType::instance(),
      I18N_NOOP( "Rotate" ),
      I18N_NOOP( "An object rotated by an angle around a point" ),
      "rotation" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_rotation" ) );

    c = new SimpleObjectTypeConstructor(
      ScalingOverCenterType::instance(),
      I18N_NOOP( "Scale" ),
      I18N_NOOP( "Scale an object over a point, by the ratio given by the length of a segment" ),
      "scale" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_scalingovercenter" ) );

    c = new SimpleObjectTypeConstructor(
      ScalingOverLineType::instance(),
      I18N_NOOP( "Scale over line" ),
      I18N_NOOP( "An object scaled over a point, by the ratio given by the length of a segment" ),
      "stretch" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_scalingoverline" ) );

    c = new SimpleObjectTypeConstructor(
      CastShadowType::instance(),
      I18N_NOOP( "Draw projective shadow" ),
      I18N_NOOP( "The shadow of an object with a given light source and projection plane (indicated by a line)" ),
      "castshadow" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_castshadow" ) );

    c = new SimpleObjectTypeConstructor(
      ProjectiveRotationType::instance(),
      I18N_NOOP( "Rotate projectively" ),
      I18N_NOOP( "An object projectively rotated by an angle and a ray" ),
      "projectiverotation" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_projectiverotation" ) );

    c = new MultiObjectTypeConstructor(
      ConicAsymptoteType::instance(),
      I18N_NOOP( "Asymptotes of a hyperbola" ),
      I18N_NOOP( "The two asymptotes of a hyperbola." ),
      "conicasymptotes", -1, 1 );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_lineconicasymptotes" ) );

    c = new ConicRadicalConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_lineconicradical") );

    c = new LocusConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_locus" ) );

    {
      // intersection type..
      // There is one "toplevel" object_constructor, that is composed
      // of multiple subconstructors..  First we build the
      // subconstructors:
      SimpleObjectTypeConstructor* lineline =
        new SimpleObjectTypeConstructor(
          LineLineIntersectionType::instance(),
          "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
          "curvelineintersection" );

      MultiObjectTypeConstructor* lineconic =
        new MultiObjectTypeConstructor(
          ConicLineIntersectionType::instance(),
          "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
          "curvelineintersection", -1, 1 );

      MultiObjectTypeConstructor* linecubic =
        new MultiObjectTypeConstructor(
          LineCubicIntersectionType::instance(),
          "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
          "curvelineintersection", 1, 2, 3 );

      // now for the toplevel constructor:
      MergeObjectConstructor* m = new MergeObjectConstructor(
        I18N_NOOP( "Intersection" ),
        I18N_NOOP( "The intersection of two objects" ),
        "curvelineintersection" );
      m->merge( lineline );
      m->merge( lineconic );
      m->merge( linecubic );
      ctors->add( m );
      actions->add( new ConstructibleAction( m, "objects_new_intersection" ) );
    };

    actions->add( new ConstructPointAction( "objects_new_normalpoint" ) );
    actions->add( new ConstructTextLabelAction( "objects_new_textlabel" ) );
  };

  done = true;
};
