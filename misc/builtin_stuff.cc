// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "builtin_stuff.h"

#include <config-kig.h>

#include "guiaction.h"
#include "lists.h"
#include "object_constructor.h"
#include "special_constructors.h"

#include "../objects/angle_type.h"
#include "../objects/arc_type.h"
#include "../objects/bezier_type.h"
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
#include "../objects/polygon_type.h"
#include "../objects/tests_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

void setupBuiltinStuff()
{
    static bool done = false;
    if (!done) {
        ObjectConstructorList *ctors = ObjectConstructorList::instance();
        GUIActionList *actions = GUIActionList::instance();
        ObjectConstructor *c = nullptr;

        // point by coords...
        c = new SimpleObjectTypeConstructor(PointByCoordsType::instance(),
                                            i18n("Point by Numeric Labels"),
                                            i18n("A point whose coordinates are given by two numeric labels"),
                                            "pointxy");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_point_by_coords"));

        // segment...
        c = new SimpleObjectTypeConstructor(SegmentABType::instance(), i18n("Segment"), i18n("A segment constructed from its start and end point"), "segment");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_segment", Qt::Key_S));

        // segment axis...
        c = new SimpleObjectTypeConstructor(SegmentAxisType::instance(),
                                            i18n("Segment Axis"),
                                            i18n("The perpendicular line through a given segment's mid point."),
                                            "segmentaxis");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_segment_axis"));

        // line by two points..
        c = new SimpleObjectTypeConstructor(LineABType::instance(), i18n("Line by Two Points"), i18n("A line constructed through two points"), "line");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_linettp", Qt::Key_L));

        // ray by two points..
        c = new SimpleObjectTypeConstructor(RayABType::instance(),
                                            i18n("Half-Line"),
                                            i18n("A half-line by its start point, and another point somewhere on it."),
                                            "ray");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_ray", Qt::Key_R));

        // perpendicular line
        c = new SimpleObjectTypeConstructor(LinePerpendLPType::instance(),
                                            i18n("Perpendicular"),
                                            i18n("A line constructed through a point, perpendicular to another line or segment."),
                                            "perpendicular");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_lineperpend"));

        // parallel line
        c = new SimpleObjectTypeConstructor(LineParallelLPType::instance(),
                                            i18n("Parallel"),
                                            i18n("A line constructed through a point, and parallel to another line or segment"),
                                            "parallel");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_lineparallel"));

        // circle
        c = new SimpleObjectTypeConstructor(CircleBCPType::instance(),
                                            i18n("Circle by Center && Point"),
                                            i18n("A circle constructed by its center and a point that pertains to it"),
                                            "circlebcp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_circlebcp", Qt::Key_C));

        c = new SimpleObjectTypeConstructor(CircleBTPType::instance(),
                                            i18n("Circle by Three Points"),
                                            i18n("A circle constructed through three points"),
                                            "circlebtp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_circlebtp"));

        c = new SimpleObjectTypeConstructor(CircleBPRType::instance(),
                                            i18n("Circle by Point && Radius"),
                                            i18n("A circle defined by its center and the length of the radius"),
                                            "circlebps");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_circlebpr"));

        // declare this object static to this function, so it gets deleted
        // at the end of the program, without us having to wonder about
        // deleting it..  We don't want to register this
        // object-constructor, because that way, "construct the bisector"
        // would appear twice in the angle popup menu: once as the generic
        // construct a property stuff, and once because of this ctor..
        // we only register the guiaction, cause it makes sense to have a
        // toolbar icon for this..
        static PropertyObjectConstructor anglebisectionctor(AngleImp::stype(),
                                                            I18N_NOOP("Construct Bisector of This Angle"),
                                                            I18N_NOOP("Select the angle you want to construct the bisector of..."),
                                                            i18n("Angle Bisector"),
                                                            i18n("The bisector of an angle"),
                                                            "angle_bisector",
                                                            "angle-bisector");
        actions->add(new ConstructibleAction(&anglebisectionctor, "objects_new_angle_bisector"));

        // conic stuff
        c = new SimpleObjectTypeConstructor(ConicB5PType::instance(),
                                            i18n("Conic by Five Points"),
                                            i18n("A conic constructed through five points"),
                                            "conicb5p");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_conicb5p"));

        c = new SimpleObjectTypeConstructor(ConicBAAPType::instance(),
                                            i18n("Hyperbola by Asymptotes && Point"),
                                            i18n("A hyperbola with given asymptotes through a point"),
                                            "conicbaap");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_conicbaap"));

        c = new SimpleObjectTypeConstructor(EllipseBFFPType::instance(),
                                            i18n("Ellipse by Focuses && Point"), // focuses is used in preference to foci
                                            i18n("An ellipse constructed by its focuses and a point that pertains to it"),
                                            "ellipsebffp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_ellipsebffp"));

        c = new SimpleObjectTypeConstructor(HyperbolaBFFPType::instance(),
                                            i18n("Hyperbola by Focuses && Point"), // focuses is used in preference to foci
                                            i18n("A hyperbola constructed by its focuses and a point that pertains to it"),
                                            "hyperbolabffp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_hyperbolabffp"));

        c = new SimpleObjectTypeConstructor(ConicBDFPType::instance(),
                                            i18n("Conic by Directrix, Focus && Point"),
                                            i18n("A conic with given directrix and focus, through a point"),
                                            "conicbdfp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_conicbdfp"));

        c = new SimpleObjectTypeConstructor(ParabolaBTPType::instance(),
                                            i18n("Vertical Parabola by Three Points"),
                                            i18n("A vertical parabola constructed through three points"),
                                            "parabolabtp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_parabolabtp"));

        c = new SimpleObjectTypeConstructor(CubicB9PType::instance(),
                                            i18n("Cubic Curve by Nine Points"),
                                            i18n("A cubic curve constructed through nine points"),
                                            "cubicb9p");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_cubicb9p"));

        c = new SimpleObjectTypeConstructor(ConicPolarPointType::instance(),
                                            i18n("Polar Point of a Line"),
                                            i18n("The polar point of a line with respect to a conic."),
                                            "polarpoint");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_pointpolar"));

        c = new SimpleObjectTypeConstructor(ConicPolarLineType::instance(),
                                            i18n("Polar Line of a Point"),
                                            i18n("The polar line of a point with respect to a conic."),
                                            "polarline");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_linepolar"));

        c = new SimpleObjectTypeConstructor(CubicNodeB6PType::instance(),
                                            i18n("Cubic Curve with Node by Six Points"),
                                            i18n("A cubic curve with a nodal point at the origin through six points"),
                                            "cubicnodeb6p");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_cubicnodeb6p"));

        c = new SimpleObjectTypeConstructor(CubicCuspB4PType::instance(),
                                            i18n("Cubic Curve with Cusp by Four Points"),
                                            i18n("A cubic curve with a horizontal cusp at the origin through four points"),
                                            "cubiccuspb4p");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_cubiccuspb4p"));

        c = new SimpleObjectTypeConstructor(VerticalCubicB4PType::instance(),
                                            i18n("Cubic Function by Four Points"),
                                            i18n("A cubic function through four points"),
                                            "verticalcubicb4p");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_verticalcubicb4p"));

        c = new SimpleObjectTypeConstructor(ConicDirectrixType::instance(), i18n("Directrix of a Conic"), i18n("The directrix line of a conic."), "directrix");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_linedirectrix"));

        c = new SimpleObjectTypeConstructor(AngleType::instance(), i18n("Angle by Three Points"), i18n("An angle defined by three points"), "angle");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_angle", Qt::Key_A));

        c = new SimpleObjectTypeConstructor(EquilateralHyperbolaB4PType::instance(),
                                            i18n("Equilateral Hyperbola by Four Points"),
                                            i18n("An equilateral hyperbola constructed through four points"),
                                            "equilateralhyperbolab4p");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_equilateralhyperbolab4p"));

        {
            // now for the Mid Point action.  It does both the mid point of
            // a segment, and the mid point of two points.  The midpoint of
            // two segments just shows the mid point property, and therefore
            // doesn't need to be added to the ctors, because there are
            // already facilities to construct an object's properties..
            // therefore, we add only an mpotp to the ctors, and add the
            // merged constructor only to the actions..
            ctors->add(new MidPointOfTwoPointsConstructor());

            ObjectConstructor *mpotp = new MidPointOfTwoPointsConstructor();
            ObjectConstructor *mpos =
                new PropertyObjectConstructor(SegmentImp::stype(), I18N_NOOP("Construct the midpoint of this segment"), "", "", "", "", "mid-point");

            // make this a static object, so it gets deleted at the end of
            // the program.
            static MergeObjectConstructor m(i18n("Mid Point"), i18n("The midpoint of a segment or two other points"), "bisection");
            m.merge(mpotp);
            m.merge(mpos);
            actions->add(new ConstructibleAction(&m, "objects_new_midpoint", Qt::Key_M));
        };

        {
            // now for the Golden Ratio Point action.  It does both the golden ratio point of
            // a segment, and the golden ratio point of two points.  The golden ratio point of
            // two segments just shows the golden ratio point property, and therefore
            // doesn't need to be added to the ctors, because there are
            // already facilities to construct an object's properties..
            // therefore, we add only an mpotp to the ctors, and add the
            // merged constructor only to the actions..
            ctors->add(new GoldenPointOfTwoPointsConstructor());

            ObjectConstructor *mpotp = new GoldenPointOfTwoPointsConstructor();
            ObjectConstructor *mpos = new PropertyObjectConstructor(SegmentImp::stype(),
                                                                    I18N_NOOP("Construct the golden ratio point of this segment"),
                                                                    "",
                                                                    "",
                                                                    "",
                                                                    "",
                                                                    "golden-point");

            // make this a static object, so it gets deleted at the end of
            // the program.
            static MergeObjectConstructor m(i18n("Golden Ratio Point"),
                                            i18n("The golden ratio point of a segment or two other points"),
                                            "segment_golden_point");
            m.merge(mpotp);
            m.merge(mpos);
            actions->add(new ConstructibleAction(&m, "objects_new_golden_point", Qt::Key_G));
        };

        c = new SimpleObjectTypeConstructor(VectorType::instance(), i18n("Vector"), i18n("Construct a vector from two given points."), "vector");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_vector", Qt::Key_V));

        c = new SimpleObjectTypeConstructor(VectorSumType::instance(), i18n("Vector Sum"), i18n("Construct the vector sum of two vectors."), "vectorsum");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_vectorsum", 0));

        c = new SimpleObjectTypeConstructor(LineByVectorType::instance(),
                                            i18n("Line by Vector"),
                                            i18n("Construct the line by a given vector though a given point."),
                                            "linebyvector");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_linebyvector", 0));

        c = new SimpleObjectTypeConstructor(HalflineByVectorType::instance(),
                                            i18n("Half-Line by Vector"),
                                            i18n("Construct the half-line by a given vector starting at given point."),
                                            "halflinebyvector");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_halflinebyvector", 0));

        c = new SimpleObjectTypeConstructor(ArcBTPType::instance(), i18n("Arc by Three Points"), i18n("Construct an arc through three points."), "arc");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_arcbtp"));

        c = new SimpleObjectTypeConstructor(ConicArcBCTPType::instance(),
                                            i18n("Conic Arc by Center and Three Points"),
                                            i18n("Construct a conic arc with given center through three points."),
                                            "conicarc");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_conicarcbctp"));

        c = new SimpleObjectTypeConstructor(ConicArcB5PType::instance(),
                                            i18n("Conic Arc by Five Points"),
                                            i18n("Construct a conic arc through five points."),
                                            "conicarc");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_conicarcb5p"));

        c = new SimpleObjectTypeConstructor(ArcBCPAType::instance(),
                                            i18n("Arc by Center, Angle && Point"),
                                            i18n("Construct an arc by its center and a given angle, "
                                                 "starting at a given point"),
                                            "arcbcpa");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_arcbcpa"));

        c = new SimpleObjectTypeConstructor(ParabolaBDPType::instance(),
                                            i18n("Parabola by Directrix && Focus"),
                                            i18n("A parabola defined by its directrix and focus"),
                                            "parabolabdp");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_parabolabdp"));

        // Transformation stuff..
        //    c = new SimpleObjectTypeConstructor(
        //      CircularInversionType::instance(),
        //      I18N_NOOP( "Invert" ),
        //      I18N_NOOP( "The inversion of an object with respect to a circle" ),
        //      "inversion" );
        //    ctors->add( c );
        //    actions->add( new ConstructibleAction( c, "objects_new_inversion" ) );

        c = new InversionConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_inversion"));

        c = new SimpleObjectTypeConstructor(TranslatedType::instance(), i18n("Translate"), i18n("The translation of an object by a vector"), "translation");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_translation"));

        c = new SimpleObjectTypeConstructor(PointReflectionType::instance(),
                                            i18n("Reflect in Point"),
                                            i18n("An object reflected in a point"),
                                            "centralsymmetry");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_pointreflection"));

        c = new SimpleObjectTypeConstructor(LineReflectionType::instance(), i18n("Reflect in Line"), i18n("An object reflected in a line"), "mirrorpoint");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_linereflection"));

        c = new SimpleObjectTypeConstructor(RotationType::instance(), i18n("Rotate"), i18n("An object rotated by an angle around a point"), "rotation");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_rotation"));

        c = new SimpleObjectTypeConstructor(ScalingOverCenterType::instance(),
                                            i18n("Scale"),
                                            i18n("Scale an object over a point, by the ratio given by the length of a segment"),
                                            "scale");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_scalingovercenter"));

        c = new SimpleObjectTypeConstructor(ScalingOverLineType::instance(),
                                            i18n("Scale over Line"),
                                            i18n("An object scaled over a line, by the ratio given by the length of a segment"),
                                            "stretch");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_scalingoverline"));

        c = new SimpleObjectTypeConstructor(ScalingOverCenter2Type::instance(),
                                            i18n("Scale (ratio given by two segments)"),
                                            i18n("Scale an object over a point, by the ratio given by the length of two segments"),
                                            "scale");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_scalingovercenter2"));

        c = new SimpleObjectTypeConstructor(ScalingOverLine2Type::instance(),
                                            i18n("Scale over Line (ratio given by two segments)"),
                                            i18n("An object scaled over a line, by the ratio given by the length of two segments"),
                                            "stretch");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_scalingoverline2"));

        c = new SimpleObjectTypeConstructor(SimilitudeType::instance(),
                                            i18n("Apply Similitude"),
                                            i18n("Apply a similitude to an object (the sequence of a scaling and rotation around a center)"),
                                            "similitude");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_similitude"));

        c = new SimpleObjectTypeConstructor(HarmonicHomologyType::instance(),
                                            i18n("Harmonic Homology"),
                                            i18n("The harmonic homology with a given center and a given axis (this is a projective transformation)"),
                                            "harmonichomology");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_harmonichomology"));

        c = new GenericAffinityConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_genericaffinity"));

        c = new GenericProjectivityConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_genericprojectivity"));

        c = new SimpleObjectTypeConstructor(CastShadowType::instance(),
                                            i18n("Draw Projective Shadow"),
                                            i18n("The shadow of an object with a given light source and projection plane (indicated by a line)"),
                                            "castshadow");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_castshadow"));

        //     c = new SimpleObjectTypeConstructor(
        //       ProjectiveRotationType::instance(),
        //       I18N_NOOP( "Rotate Projectively" ),
        //       I18N_NOOP( "An object projectively rotated by an angle and a half-line" ),
        //       "projectiverotation" );
        //     ctors->add( c );
        //     actions->add( new ConstructibleAction( c, "objects_new_projectiverotation" ) );

        c = new MultiObjectTypeConstructor(ConicAsymptoteType::instance(),
                                           i18n("Asymptotes of a Hyperbola"),
                                           i18n("The two asymptotes of a hyperbola."),
                                           "conicasymptotes",
                                           -1,
                                           1);
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_lineconicasymptotes"));

        c = new ConicRadicalConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_lineconicradical"));

        /* ----------- start polygons --------- */

        c = new SimpleObjectTypeConstructor(TriangleB3PType::instance(),
                                            i18n("Triangle by Its Vertices"),
                                            i18n("Construct a triangle given its three vertices."),
                                            "triangle");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_trianglebtp"));

        c = new PolygonBNPTypeConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_polygonbnp"));

        c = new OpenPolygonTypeConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_openpolygon"));

        c = new PolygonBCVConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_polygonbcv"));

        c = new PolygonVertexTypeConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_polygonvertices"));

        c = new PolygonSideTypeConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_polygonsides"));

        c = new SimpleObjectTypeConstructor(ConvexHullType::instance(),
                                            i18n("Convex Hull"),
                                            i18n("A polygon that corresponds to the convex hull of another polygon"),
                                            "convexhull");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_convexhull"));

        /* ----------- end polygons --------- */

        /* ----------- start bezier --------- */

        c = new SimpleObjectTypeConstructor(BezierQuadricType::instance(),
                                            i18n("Bézier Quadratic by its Control Points"),
                                            i18n("Construct a Bézier quadratic given its three control points."),
                                            "bezier3");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_bezierquadratic"));

        c = new SimpleObjectTypeConstructor(BezierCubicType::instance(),
                                            i18n("Bézier Cubic by its Control Points"),
                                            i18n("Construct a Bézier cubic given its four control points."),
                                            "bezier4");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_beziercubic"));

        c = new BezierCurveTypeConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_beziercurve"));

        c = new SimpleObjectTypeConstructor(RationalBezierQuadricType::instance(),
                                            i18n("Rational Bézier Quadratic by its Control Points"),
                                            i18n("Construct a Rational Bézier quadratic given its three control points."),
                                            "rbezier3");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_rationalbezierquadratic"));

        c = new SimpleObjectTypeConstructor(RationalBezierCubicType::instance(),
                                            i18n("Rational Bézier Cubic by its Control Points"),
                                            i18n("Construct a Rational Bézier cubic given its four control points."),
                                            "rbezier4");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_rationalbeziercubic"));

        c = new RationalBezierCurveTypeConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_rationalbeziercurve"));

        /* ----------- end bezier ----------- */

        c = new LocusConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_locus"));

        // tests
        c = new TestConstructor(AreParallelType::instance(), i18n("Parallel Test"), i18n("Test whether two given lines are parallel"), "testparallel");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_areparallel"));

        c = new TestConstructor(AreOrthogonalType::instance(), i18n("Orthogonal Test"), i18n("Test whether two given lines are orthogonal"), "testorthogonal");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_areorthogonal"));

        c = new TestConstructor(AreCollinearType::instance(), i18n("Collinear Test"), i18n("Test whether three given points are collinear"), "testcollinear");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_arecollinear"));

        c = new TestConstructor(ContainsTestType::instance(), i18n("Contains Test"), i18n("Test whether a given curve contains a given point"), "testcontains");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_containstest"));

        c = new TestConstructor(InPolygonTestType::instance(), i18n("In Polygon Test"), i18n("Test whether a given polygon contains a given point"), "test");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_inpolygontest"));

        c = new TestConstructor(ConvexPolygonTestType::instance(), i18n("Convex Polygon Test"), i18n("Test whether a given polygon is convex"), "test");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_convexpolygontest"));

        c = new TestConstructor(ExistenceTestType::instance(), i18n("Existence Test"), i18n("Test whether a given object is constructible"), "test");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_existencetest"));

        c = new TestConstructor(SameDistanceType::instance(),
                                i18n("Distance Test"),
                                i18n("Test whether a given point have the same distance from a given point "
                                     "and from another given point"),
                                "testdistance");
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_distancetest"));

        c = new TestConstructor(VectorEqualityTestType::instance(), i18n("Vector Equality Test"), i18n("Test whether two vectors are equal"), "test");
        //      "testequal" );
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_vectorequalitytest"));

        c = new MeasureTransportConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_measuretransport"));

        //    c = new SimpleObjectTypeConstructor(
        //      MeasureTransportType::instance(),
        //      I18N_NOOP( "Measure Transport" ),
        //      I18N_NOOP( "Transport the measure of a segment or arc over a line or circle." ),
        //      "measuretransport" );
        //    ctors->add( c );
        //    actions->add( new ConstructibleAction( c, "objects_new_measuretransport" ) );

        c = new SimpleObjectTypeConstructor(ProjectedPointType::instance(),
                                            i18n("Point Projection"),
                                            i18n("Project a point on a line"),
                                            QStringLiteral("projection"));
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_projection"));

        // the generic intersection constructor..
        c = new GenericIntersectionConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_intersection", Qt::Key_I));

        // the generic tangent constructor
        c = new TangentConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_tangent", Qt::Key_T));

        // the generic center of curvature constructor
        c = new CocConstructor();
        ctors->add(c);
        actions->add(new ConstructibleAction(c, "objects_new_centerofcurvature"));

        actions->add(new ConstructPointAction("objects_new_normalpoint"));
        actions->add(new ConstructTextLabelAction("objects_new_textlabel"));
        actions->add(new AddFixedPointAction("objects_new_point_xy"));
        actions->add(new ConstructNumericLabelAction("objects_new_numericlabel"));

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/script-common.h"
        actions->add(
            new NewScriptAction(I18N_NOOP("Python Script"), I18N_NOOP("Construct a new Python script."), "objects_new_script_python", ScriptType::Python));
#endif

#if 0
    actions->add( new TestAction( "test_stuff" ) );
#endif
    };

    done = true;
}
