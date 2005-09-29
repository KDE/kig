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

#include "builtin_stuff.h"

#include <config.h>

#include "object_constructor.h"
#include "lists.h"
#include "special_constructors.h"
#include "guiaction.h"

#include "../objects/angle_type.h"
#include "../objects/arc_type.h"
#include "../objects/circle_type.h"
#include "../objects/conic_types.h"
#include "../objects/cubic_type.h"
#include "../objects/intersection_types.h"
#include "../objects/inversion_type.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/object_imp.h"
#include "../objects/other_imp.h"
#include "../objects/other_type.h"
#include "../objects/point_type.h"
#include "../objects/tests_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"
#include "../objects/polygon_type.h"

#include <klocale.h>

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
    actions->add( new ConstructibleAction( c, "objects_new_segment", Qt::Key_S ) );

    // line by two points..
    c = new SimpleObjectTypeConstructor(
      LineABType::instance(), I18N_NOOP( "Line by Two Points" ),
      I18N_NOOP( "A line constructed through two points"), "line" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linettp", Qt::Key_L ) );

    // ray by two points..
    c = new SimpleObjectTypeConstructor(
      RayABType::instance(), I18N_NOOP( "Half-Line" ),
      I18N_NOOP( "A half-line by its start point, and another point somewhere on it." ),
      "ray" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_ray", Qt::Key_R ) );

    // perpendicular line
    c = new SimpleObjectTypeConstructor(
      LinePerpendLPType::instance(), I18N_NOOP( "Perpendicular" ),
      I18N_NOOP( "A line constructed through a point, perpendicular to another line or segment." ),
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
      CircleBCPType::instance(), I18N_NOOP( "Circle by Center && Point" ),
      I18N_NOOP( "A circle constructed by its center and a point that pertains to it" ),
      "circlebcp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_circlebcp", Qt::Key_C ) );

    c = new SimpleObjectTypeConstructor(
      CircleBTPType::instance(), I18N_NOOP( "Circle by Three Points" ),
      I18N_NOOP( "A circle constructed through three points" ),
      "circlebtp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_circlebtp" ) );

    // declare this object static to this function, so it gets deleted
    // at the end of the program, without us having to wonder about
    // deleting it..  We don't want to register this
    // object-constructor, because that way, "construct the bisector"
    // would appear twice in the angle popup menu: once as the generic
    // construct a property stuff, and once because of this ctor..
    // we only register the guiaction, cause it makes sense to have a
    // toolbar icon for this..
    static PropertyObjectConstructor anglebisectionctor(
      AngleImp::stype(),
      I18N_NOOP( "Construct Bisector of This Angle" ),
      I18N_NOOP( "Select the angle you want to construct the bisector of..." ),
      I18N_NOOP( "Angle Bisector" ),
      I18N_NOOP( "The bisector of an angle" ),
      "angle_bisector",
      "angle-bisector" );
    actions->add( new ConstructibleAction( &anglebisectionctor, "objects_new_angle_bisector" ) );

    // conic stuff
    c = new SimpleObjectTypeConstructor(
      ConicB5PType::instance(), I18N_NOOP( "Conic by Five Points" ),
      I18N_NOOP( "A conic constructed through five points" ),
      "conicb5p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicb5p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicBAAPType::instance(),
      I18N_NOOP( "Hyperbola by Asymptotes && Point" ),
      I18N_NOOP( "A hyperbola with given asymptotes through a point" ),
      "conicbaap" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_conicbaap" ) );

    c = new SimpleObjectTypeConstructor(
      EllipseBFFPType::instance(),
      I18N_NOOP( "Ellipse by Focuses && Point" ), // focuses is used in preference to foci
      I18N_NOOP( "An ellipse constructed by its focuses and a point that pertains to it" ),
      "ellipsebffp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_ellipsebffp" ) );

    c = new SimpleObjectTypeConstructor(
      HyperbolaBFFPType::instance(),
      I18N_NOOP( "Hyperbola by Focuses && Point" ), // focuses is used in preference to foci
      I18N_NOOP( "A hyperbola constructed by its focuses and a point that pertains to it" ),
      "hyperbolabffp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_hyperbolabffp" ) );

    c = new SimpleObjectTypeConstructor(
      ConicBDFPType::instance(),
      I18N_NOOP( "Conic by Directrix, Focus && Point" ),
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
      I18N_NOOP( "Cubic Curve by Nine Points" ),
      I18N_NOOP( "A cubic curve constructed through nine points" ),
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
      I18N_NOOP( "Cubic Curve with Node by Six Points" ),
      I18N_NOOP( "A cubic curve with a nodal point at the origin through six points" ),
      "cubicnodeb6p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_cubicnodeb6p" ) );

    c = new SimpleObjectTypeConstructor(
      CubicCuspB4PType::instance(),
      I18N_NOOP( "Cubic Curve with Cusp by Four Points" ),
      I18N_NOOP( "A cubic curve with a horizontal cusp at the origin through four points" ),
      "cubiccuspb4p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_cubiccuspb4p" ) );

    c = new SimpleObjectTypeConstructor(
      ConicDirectrixType::instance(),
      I18N_NOOP( "Directrix of a Conic" ),
      I18N_NOOP( "The directrix line of a conic." ),
      "directrix" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linedirectrix" ) );

    c = new SimpleObjectTypeConstructor(
      AngleType::instance(),
      I18N_NOOP( "Angle by Three Points" ),
      I18N_NOOP( "An angle defined by three points" ),
      "angle" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_angle", Qt::Key_A ) );

    c = new SimpleObjectTypeConstructor(
      EquilateralHyperbolaB4PType::instance(),
      I18N_NOOP( "Equilateral Hyperbola by Four Points" ),
      I18N_NOOP( "An equilateral hyperbola constructed through four points" ),
      "equilateralhyperbolab4p" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_equilateralhyperbolab4p" ) );

    {
      // now for the Mid Point action.  It does both the mid point of
      // a segment, and the mid point of two points.  The midpoint of
      // two segments just shows the mid point property, and therefore
      // doesn't need to be added to the ctors, because there are
      // already facilities to construct an object's properties..
      // therefore, we add only an mpotp to the ctors, and add the
      // merged constructor only to the actions..
      ctors->add( new MidPointOfTwoPointsConstructor() );

      ObjectConstructor* mpotp = new MidPointOfTwoPointsConstructor();
      ObjectConstructor* mpos = new PropertyObjectConstructor(
        SegmentImp::stype(), I18N_NOOP( "Construct the midpoint of this segment" ),
        "", "", "", "", "mid-point" );

      // make this a static object, so it gets deleted at the end of
      // the program.
      static MergeObjectConstructor m(
        I18N_NOOP( "Mid Point" ),
        I18N_NOOP( "The midpoint of a segment or two other points" ),
        "bisection" );
      m.merge( mpotp );
      m.merge( mpos );
      actions->add( new ConstructibleAction( &m, "objects_new_midpoint", Qt::Key_M ) );
    };

    c = new SimpleObjectTypeConstructor(
      VectorType::instance(),
      I18N_NOOP( "Vector" ),
      I18N_NOOP( "Construct a vector from two given points." ),
      "vector" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_vector", Qt::Key_V ) );

    c = new SimpleObjectTypeConstructor(
      VectorSumType::instance(),
      I18N_NOOP( "Vector Sum" ),
      I18N_NOOP( "Construct the vector sum of two vectors." ),
      "vectorsum" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_vectorsum", 0 ) );

    c = new SimpleObjectTypeConstructor(
      LineByVectorType::instance(),
      I18N_NOOP( "Line by Vector" ),
      I18N_NOOP( "Construct the line by a given vector though a given point." ),
      "linebyvector" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_linebyvector", 0 ) );

    c = new SimpleObjectTypeConstructor(
      HalflineByVectorType::instance(),
      I18N_NOOP( "Half-Line by Vector" ),
      I18N_NOOP( "Construct the half-line by a given vector starting at given point." ),
      "halflinebyvector" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_halflinebyvector", 0 ) );

    c = new SimpleObjectTypeConstructor(
      ArcBTPType::instance(),
      I18N_NOOP( "Arc by Three Points" ),
      I18N_NOOP( "Construct an arc through three points." ),
      "arc" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_arcbtp" ) );

    c = new SimpleObjectTypeConstructor(
      ArcBCPAType::instance(),
      I18N_NOOP( "Arc by Center, Angle && Point" ),
      I18N_NOOP( "Construct an arc by its center and a given angle, "
                 "starting at a given point" ),
      "arcbcpa" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_arcbcpa" ) );

    c = new SimpleObjectTypeConstructor(
      ParabolaBDPType::instance(),
      I18N_NOOP( "Parabola by Directrix && Focus" ),
      I18N_NOOP( "A parabola defined by its directrix and focus" ),
      "parabolabdp" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_parabolabdp" ) );

    // Transformation stuff..
    c = new InversionConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_inversion" ) );

    c = new SimpleObjectTypeConstructor(
      TranslatedType::instance(),
      I18N_NOOP( "Translate" ),
      I18N_NOOP( "The translation of an object by a vector" ),
      "translation" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_translation" ) );

    c = new SimpleObjectTypeConstructor(
      PointReflectionType::instance(),
      I18N_NOOP( "Reflect in Point" ),
      I18N_NOOP( "An object reflected in a point" ),
      "centralsymmetry" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_pointreflection" ) );

    c = new SimpleObjectTypeConstructor(
      LineReflectionType::instance(),
      I18N_NOOP( "Reflect in Line" ),
      I18N_NOOP( "An object reflected in a line" ),
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
      I18N_NOOP( "Scale over Line" ),
      I18N_NOOP( "An object scaled over a line, by the ratio given by the length of a segment" ),
      "stretch" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_scalingoverline" ) );

    c = new SimpleObjectTypeConstructor(
      ScalingOverCenter2Type::instance(),
      I18N_NOOP( "Scale (ratio given by two segments)" ),
      I18N_NOOP( "Scale an object over a point, by the ratio given by the length of two segments" ),
      "scale" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_scalingovercenter2" ) );

    c = new SimpleObjectTypeConstructor(
      ScalingOverLine2Type::instance(),
      I18N_NOOP( "Scale over Line (ratio given by two segments)" ),
      I18N_NOOP( "An object scaled over a line, by the ratio given by the length of two segments" ),
      "stretch" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_scalingoverline2" ) );

    c = new SimpleObjectTypeConstructor(
      SimilitudeType::instance(),
      I18N_NOOP( "Apply Similitude" ),
      I18N_NOOP( "Apply a similitude to an object ( the sequence of a scaling and rotation around a center )" ),
      "similitude" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_similitude" ) );

    c = new SimpleObjectTypeConstructor(
      HarmonicHomologyType::instance(),
      I18N_NOOP( "Harmonic Homology" ),
      I18N_NOOP( "The harmonic homology with a given center and a given axis (this is a projective transformation)" ),
      "harmonichomology" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_harmonichomology" ) );

    c = new GenericAffinityConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_genericaffinity" ) );

    c = new GenericProjectivityConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_genericprojectivity" ) );

    c = new SimpleObjectTypeConstructor(
      CastShadowType::instance(),
      I18N_NOOP( "Draw Projective Shadow" ),
      I18N_NOOP( "The shadow of an object with a given light source and projection plane (indicated by a line)" ),
      "castshadow" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_castshadow" ) );

//     c = new SimpleObjectTypeConstructor(
//       ProjectiveRotationType::instance(),
//       I18N_NOOP( "Rotate Projectively" ),
//       I18N_NOOP( "An object projectively rotated by an angle and a half-line" ),
//       "projectiverotation" );
//     ctors->add( c );
//     actions->add( new ConstructibleAction( c, "objects_new_projectiverotation" ) );

    c = new MultiObjectTypeConstructor(
      ConicAsymptoteType::instance(),
      I18N_NOOP( "Asymptotes of a Hyperbola" ),
      I18N_NOOP( "The two asymptotes of a hyperbola." ),
      "conicasymptotes", -1, 1 );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_lineconicasymptotes" ) );

    c = new ConicRadicalConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_lineconicradical") );

    /* ----------- start polygons --------- */

    c = new SimpleObjectTypeConstructor(
      TriangleB3PType::instance(),
      I18N_NOOP( "Triangle by Its Vertices" ),
      I18N_NOOP( "Construct a triangle given its three vertices." ),
      "triangle" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_trianglebtp" ) );

    c = new PolygonBNPTypeConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_polygonbnp" ));

    c = new PolygonBCVConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_polygonbcv" ) );

    c = new PolygonVertexTypeConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_polygonvertices" ));

    c = new PolygonSideTypeConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_polygonsides" ));

    c = new SimpleObjectTypeConstructor(
      ConvexHullType::instance(), I18N_NOOP( "Convex Hull" ),
      I18N_NOOP( "A polygon that corresponds to the convex hull of another polygon" ),
      "convexhull" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_convexhull" ) );

    /* ----------- end polygons --------- */

    c = new LocusConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_locus" ) );

    // tests
    c = new TestConstructor(
      AreParallelType::instance(),
      I18N_NOOP( "Parallel Test" ),
      I18N_NOOP( "Test whether two given lines are parallel" ),
      "testparallel" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_areparallel" ) );

    c = new TestConstructor(
      AreOrthogonalType::instance(),
      I18N_NOOP( "Orthogonal Test" ),
      I18N_NOOP( "Test whether two given lines are orthogonal" ),
      "testorthogonal" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_areorthogonal" ) );

    c = new TestConstructor(
      AreCollinearType::instance(),
      I18N_NOOP( "Collinear Test" ),
      I18N_NOOP( "Test whether three given points are collinear" ),
      "testcollinear" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_arecollinear" ) );

    c = new TestConstructor(
      ContainsTestType::instance(),
      I18N_NOOP( "Contains Test" ),
      I18N_NOOP( "Test whether a given curve contains a given point" ),
      "testcontains" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_containstest" ) );

    c = new TestConstructor(
      InPolygonTestType::instance(),
      I18N_NOOP( "In Polygon Test" ),
      I18N_NOOP( "Test whether a given polygon contains a given point" ),
      "test" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_inpolygontest" ) );

    c = new TestConstructor(
      ConvexPolygonTestType::instance(),
      I18N_NOOP( "Convex Polygon Test" ),
      I18N_NOOP( "Test whether a given polygon is convex" ),
      "test" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_convexpolygontest" ) );

    c = new TestConstructor(
      SameDistanceType::instance(),
      I18N_NOOP( "Distance Test" ),
      I18N_NOOP( "Test whether a given point have the same distance from a given point "
                 "and from another given point" ),
      "testdistance" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_distancetest" ) );

    c = new TestConstructor(
      VectorEqualityTestType::instance(),
      I18N_NOOP( "Vector Equality Test" ),
      I18N_NOOP( "Test whether two vectors are equal" ),
      "test" );
//      "testequal" );
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_vectorequalitytest" ) );

    c = new MeasureTransportConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_measuretransport" ));

//    c = new SimpleObjectTypeConstructor(
//      MeasureTransportType::instance(),
//      I18N_NOOP( "Measure Transport" ),
//      I18N_NOOP( "Transport the measure of a segment or arc over a line or circle." ),
//      "measuretransport" );
//    ctors->add( c );
//    actions->add( new ConstructibleAction( c, "objects_new_measuretransport" ) );

    // the generic intersection constructor..
    c = new GenericIntersectionConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_intersection", Qt::Key_I ) );

    // the generic tangent constructor
    c = new TangentConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_tangent", Qt::Key_T ) );

    // the generic center of curvature constructor
    c = new CocConstructor();
    ctors->add( c );
    actions->add( new ConstructibleAction( c, "objects_new_centerofcurvature" ) );

    actions->add( new ConstructPointAction( "objects_new_normalpoint" ) );
    actions->add( new ConstructTextLabelAction( "objects_new_textlabel" ) );
    actions->add( new AddFixedPointAction( "objects_new_point_xy" ) );

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/script-common.h"
    actions->add( new NewScriptAction(
                        I18N_NOOP( "Python Script" ),
                        I18N_NOOP( "Construct a new Python script." ),
                        "objects_new_script_python",
                        ScriptType::Python ) );
#endif

#if 0
    actions->add( new TestAction( "test_stuff" ) );
#endif
  };

  done = true;
}
