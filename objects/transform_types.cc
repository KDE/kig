// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "transform_types.h"

#include <math.h>

#include "../misc/coordinate.h"
#include "../misc/kigtransform.h"
#include "bogus_imp.h"
#include "line_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "polygon_imp.h"
#include "special_imptypes.h"

#include <cmath>

static const ArgsParser::spec argsspecTranslation[] = {
    {ObjectImp::stype(), kli18n("Translate this object"), kli18n("Select the object to translate..."), false},
    {VectorImp::stype(), kli18n("Translate by this vector"), kli18n("Select the vector to translate by..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(TranslatedType)

TranslatedType::TranslatedType()
    : ArgsParserObjectType("Translation", argsspecTranslation, 2)
{
}

TranslatedType::~TranslatedType()
{
}

const TranslatedType *TranslatedType::instance()
{
    static const TranslatedType t;
    return &t;
}

ObjectImp *TranslatedType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate dir = static_cast<const VectorImp *>(args[1])->dir();
    Transformation t = Transformation::translation(dir);

    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecPointReflection[] = {
    {ObjectImp::stype(), kli18n("Reflect this object"), kli18n("Select the object to reflect..."), false},
    {PointImp::stype(), kli18n("Reflect in this point"), kli18n("Select the point to reflect in..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(PointReflectionType)

PointReflectionType::PointReflectionType()
    : ArgsParserObjectType("PointReflection", argsspecPointReflection, 2)
{
}

PointReflectionType::~PointReflectionType()
{
}

const PointReflectionType *PointReflectionType::instance()
{
    static const PointReflectionType t;
    return &t;
}

ObjectImp *PointReflectionType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate center = static_cast<const PointImp *>(args[1])->coordinate();
    Transformation t = Transformation::pointReflection(center);

    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecLineReflection[] = {
    {ObjectImp::stype(), kli18n("Reflect this object"), kli18n("Select the object to reflect..."), false},
    {AbstractLineImp::stype(), kli18n("Reflect in this line"), kli18n("Select the line to reflect in..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(LineReflectionType)

LineReflectionType::LineReflectionType()
    : ArgsParserObjectType("LineReflection", argsspecLineReflection, 2)
{
}

LineReflectionType::~LineReflectionType()
{
}

const LineReflectionType *LineReflectionType::instance()
{
    static const LineReflectionType t;
    return &t;
}

ObjectImp *LineReflectionType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    LineData d = static_cast<const AbstractLineImp *>(args[1])->data();
    Transformation t = Transformation::lineReflection(d);

    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecRotation[] = {
    {ObjectImp::stype(), kli18n("Rotate this object"), kli18n("Select the object to rotate..."), false},
    {PointImp::stype(), kli18n("Rotate around this point"), kli18n("Select the center point of the rotation..."), false},
    //  { AngleImp::stype(), kli18n( "Rotate by this angle" ),
    //    kli18n( "Select the angle of the rotation..." ), false }
    {&angleimptypeinstance, kli18n("Rotate by this angle"), kli18n("Select the angle of the rotation..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(RotationType)

RotationType::RotationType()
    : ArgsParserObjectType("Rotation", argsspecRotation, 3)
{
}

RotationType::~RotationType()
{
}

const RotationType *RotationType::instance()
{
    static const RotationType t;
    return &t;
}

ObjectImp *RotationType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate center = static_cast<const PointImp *>(args[1])->coordinate();
    //  double angle = static_cast<const AngleImp*>( args[2] )->size();
    bool valid;
    double angle = getDoubleFromImp(args[2], valid);
    if (!valid)
        return new InvalidImp;

    return args[0]->transform(Transformation::rotation(angle, center));
}

static const ArgsParser::spec argsspecScalingOverCenter[] = {
    {ObjectImp::stype(), kli18n("Scale this object"), kli18n("Select the object to scale..."), false},
    {PointImp::stype(), kli18n("Scale with this center"), kli18n("Select the center point of the scaling..."), false},
    //  { SegmentImp::stype(), kli18n( "Scale by the length of this segment" ),
    //    kli18n( "Select a segment whose length is the factor of the scaling..." ), false }
    {&lengthimptypeinstance, kli18n("Scale by this length"), kli18n("Select a length or a segment whose length is the factor of the scaling..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ScalingOverCenterType)

ScalingOverCenterType::ScalingOverCenterType()
    : ArgsParserObjectType("ScalingOverCenter", argsspecScalingOverCenter, 3)
{
}

ScalingOverCenterType::~ScalingOverCenterType()
{
}

const ScalingOverCenterType *ScalingOverCenterType::instance()
{
    static const ScalingOverCenterType t;
    return &t;
}

ObjectImp *ScalingOverCenterType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate center = static_cast<const PointImp *>(args[1])->coordinate();
    bool valid;
    double ratio = getDoubleFromImp(args[2], valid);
    if (!valid)
        return new InvalidImp;

    return args[0]->transform(Transformation::scalingOverPoint(ratio, center));
}

static const ArgsParser::spec argsspecScalingOverCenter2[] = {
    {ObjectImp::stype(), kli18n("Scale this object"), kli18n("Select the object to scale..."), false},
    {PointImp::stype(), kli18n("Scale with this center"), kli18n("Select the center point of the scaling..."), false},
    //  { SegmentImp::stype(), kli18n( "Scale the length of this segment..." ),
    //    kli18n( "Select the first of two segments whose ratio is the factor of the scaling..." ), false },
    //  { SegmentImp::stype(), kli18n( "...to the length of this other segment" ),
    //    kli18n( "Select the second of two segments whose ratio is the factor of the scaling..." ), false }
    {&lengthimptypeinstance,
     kli18n("Scale this length..."),
     kli18n("Select the first of two lengths whose ratio is the factor of the scaling..."),
     false},
    {&lengthimptypeinstance,
     kli18n("...to this other length"),
     kli18n("Select the second of two lengths whose ratio is the factor of the scaling..."),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ScalingOverCenter2Type)

ScalingOverCenter2Type::ScalingOverCenter2Type()
    : ArgsParserObjectType("ScalingOverCenter2", argsspecScalingOverCenter2, 4)
{
}

ScalingOverCenter2Type::~ScalingOverCenter2Type()
{
}

const ScalingOverCenter2Type *ScalingOverCenter2Type::instance()
{
    static const ScalingOverCenter2Type t;
    return &t;
}

ObjectImp *ScalingOverCenter2Type::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate center = static_cast<const PointImp *>(args[1])->coordinate();
    //  double ratio = static_cast<const SegmentImp*>( args[3] )->length()/
    //                 static_cast<const SegmentImp*>( args[2] )->length();
    bool valid;
    double denom = getDoubleFromImp(args[2], valid);
    if (!valid || denom == 0.0)
        return new InvalidImp;
    double ratio = getDoubleFromImp(args[3], valid) / denom;
    if (!valid)
        return new InvalidImp;

    return args[0]->transform(Transformation::scalingOverPoint(ratio, center));
}

static const ArgsParser::spec argsspecScalingOverLine[] = {
    {ObjectImp::stype(), kli18n("Scale this object"), kli18n("Select the object to scale"), false},
    {AbstractLineImp::stype(), kli18n("Scale over this line"), kli18n("Select the line to scale over"), false},
    //  { SegmentImp::stype(), kli18n( "Scale by the length of this segment" ), kli18n( "Select a segment whose length is the factor for the scaling" ),
    //  false }
    {&lengthimptypeinstance, kli18n("Scale by this length"), kli18n("Select a length or a segment whose length is the factor of the scaling..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ScalingOverLineType)

ScalingOverLineType::ScalingOverLineType()
    : ArgsParserObjectType("ScalingOverLine", argsspecScalingOverLine, 3)
{
}

ScalingOverLineType::~ScalingOverLineType()
{
}

const ScalingOverLineType *ScalingOverLineType::instance()
{
    static const ScalingOverLineType t;
    return &t;
}

ObjectImp *ScalingOverLineType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    LineData line = static_cast<const AbstractLineImp *>(args[1])->data();
    bool valid;
    double ratio = getDoubleFromImp(args[2], valid);
    if (!valid)
        return new InvalidImp;

    return args[0]->transform(Transformation::scalingOverLine(ratio, line));
}

static const ArgsParser::spec argsspecScalingOverLine2[] = {
    {ObjectImp::stype(), kli18n("Scale this object"), kli18n("Select the object to scale"), false},
    {AbstractLineImp::stype(), kli18n("Scale over this line"), kli18n("Select the line to scale over"), false},
    //  { SegmentImp::stype(), kli18n( "Scale the length of this segment..." ), kli18n( "Select the first of two segments whose ratio is the factor for
    //  the scaling" ), false }, { SegmentImp::stype(), kli18n( "...to the length of this segment" ), kli18n( "Select the second of two segments whose
    //  ratio is the factor for the scaling" ), false }
    {&lengthimptypeinstance,
     kli18n("Scale this length..."),
     kli18n("Select the first of two lengths whose ratio is the factor of the scaling..."),
     false},
    {&lengthimptypeinstance,
     kli18n("...to this other length"),
     kli18n("Select the second of two lengths whose ratio is the factor of the scaling..."),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ScalingOverLine2Type)

ScalingOverLine2Type::ScalingOverLine2Type()
    : ArgsParserObjectType("ScalingOverLine2", argsspecScalingOverLine2, 4)
{
}

ScalingOverLine2Type::~ScalingOverLine2Type()
{
}

const ScalingOverLine2Type *ScalingOverLine2Type::instance()
{
    static const ScalingOverLine2Type t;
    return &t;
}

ObjectImp *ScalingOverLine2Type::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    LineData line = static_cast<const AbstractLineImp *>(args[1])->data();
    //  double ratio = static_cast<const SegmentImp*>( args[3] )->length()/
    //                 static_cast<const SegmentImp*>( args[2] )->length();
    bool valid;
    double denom = getDoubleFromImp(args[2], valid);
    if (!valid || denom == 0.0)
        return new InvalidImp;
    double ratio = getDoubleFromImp(args[3], valid) / denom;
    if (!valid)
        return new InvalidImp;

    return args[0]->transform(Transformation::scalingOverLine(ratio, line));
}

static const ArgsParser::spec argsspecProjectiveRotation[] = {
    {ObjectImp::stype(), kli18n("Projectively rotate this object"), kli18n("Select the object to rotate projectively"), false},
    {RayImp::stype(),
     kli18n("Projectively rotate with this half-line"),
     kli18n("Select the half line of the projective rotation that you want to apply to the object"),
     false},
    {AngleImp::stype(),
     kli18n("Projectively rotate by this angle"),
     kli18n("Select the angle of the projective rotation that you want to apply to the object"),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ProjectiveRotationType)

ProjectiveRotationType::ProjectiveRotationType()
    : ArgsParserObjectType("ProjectiveRotation", argsspecProjectiveRotation, 3)
{
}

ProjectiveRotationType::~ProjectiveRotationType()
{
}

const ProjectiveRotationType *ProjectiveRotationType::instance()
{
    static const ProjectiveRotationType t;
    return &t;
}

ObjectImp *ProjectiveRotationType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    const RayImp *ray = static_cast<const RayImp *>(args[1]);
    Coordinate c1 = ray->data().a;
    Coordinate dir = ray->data().dir().normalize();
    double alpha = static_cast<const AngleImp *>(args[2])->size();

    return args[0]->transform(Transformation::projectiveRotation(alpha, dir, c1));
}

static const ArgsParser::spec argsspecHarmonicHomology[] = {
    {ObjectImp::stype(), kli18n("Harmonic Homology of this object"), kli18n("Select the object to transform..."), false},
    {PointImp::stype(), kli18n("Harmonic Homology with this center"), kli18n("Select the center point of the harmonic homology..."), false},
    {AbstractLineImp::stype(), kli18n("Harmonic Homology with this axis"), kli18n("Select the axis of the harmonic homology..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(HarmonicHomologyType)

HarmonicHomologyType::HarmonicHomologyType()
    : ArgsParserObjectType("HarmonicHomology", argsspecHarmonicHomology, 3)
{
}

HarmonicHomologyType::~HarmonicHomologyType()
{
}

const HarmonicHomologyType *HarmonicHomologyType::instance()
{
    static const HarmonicHomologyType t;
    return &t;
}

ObjectImp *HarmonicHomologyType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate center = static_cast<const PointImp *>(args[1])->coordinate();
    LineData axis = static_cast<const AbstractLineImp *>(args[2])->data();
    return args[0]->transform(Transformation::harmonicHomology(center, axis));
}

static const ArgsParser::spec argsspecAffinityB2Tr[] = {
    {ObjectImp::stype(), kli18n("Generic affinity of this object"), kli18n("Select the object to transform..."), false},
    {FilledPolygonImp::stype3(), kli18n("Map this triangle"), kli18n("Select the triangle that has to be transformed onto a given triangle..."), false},
    {FilledPolygonImp::stype3(),
     kli18n("onto this other triangle"),
     kli18n("Select the triangle that is the image by the affinity of the first triangle..."),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(AffinityB2TrType)

AffinityB2TrType::AffinityB2TrType()
    : ArgsParserObjectType("AffinityB2Tr", argsspecAffinityB2Tr, 3)
{
}

AffinityB2TrType::~AffinityB2TrType()
{
}

const AffinityB2TrType *AffinityB2TrType::instance()
{
    static const AffinityB2TrType t;
    return &t;
}

ObjectImp *AffinityB2TrType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    std::vector<Coordinate> frompoints = static_cast<const FilledPolygonImp *>(args[1])->points();
    std::vector<Coordinate> topoints = static_cast<const FilledPolygonImp *>(args[2])->points();

    bool valid = true;
    Transformation t = Transformation::affinityGI3P(frompoints, topoints, valid);

    if (valid == false)
        return new InvalidImp;
    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecAffinityGI3P[] = {
    {ObjectImp::stype(), kli18n("Generic affinity of this object"), kli18n("Select the object to transform..."), false},
    {PointImp::stype(), kli18n("First of 3 starting points"), kli18n("Select the first of the three starting points of the generic affinity..."), false},
    {PointImp::stype(),
     kli18n("Second of 3 starting points"),
     kli18n("Select the second of the three starting points of the generic affinity..."),
     false},
    {PointImp::stype(), kli18n("Third of 3 starting points"), kli18n("Select the third of the three starting points of the generic affinity..."), false},
    {PointImp::stype(),
     kli18n("Transformed position of first point"),
     kli18n("Select the first of the three end points of the generic affinity..."),
     false},
    {PointImp::stype(),
     kli18n("Transformed position of second point"),
     kli18n("Select the second of the three end points of the generic affinity..."),
     false},
    {PointImp::stype(),
     kli18n("Transformed position of third point"),
     kli18n("Select the third of the three end points of the generic affinity..."),
     false},
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(AffinityGI3PType)

AffinityGI3PType::AffinityGI3PType()
    : ArgsParserObjectType("AffinityGI3P", argsspecAffinityGI3P, 7)
{
}

AffinityGI3PType::~AffinityGI3PType()
{
}

const AffinityGI3PType *AffinityGI3PType::instance()
{
    static const AffinityGI3PType t;
    return &t;
}

ObjectImp *AffinityGI3PType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    std::vector<Coordinate> frompoints;
    std::vector<Coordinate> topoints;
    for (uint i = 0; i < 3; ++i) {
        frompoints.push_back(static_cast<const PointImp *>(args[i + 1])->coordinate());
        topoints.push_back(static_cast<const PointImp *>(args[i + 4])->coordinate());
    }

    bool valid = true;
    Transformation t = Transformation::affinityGI3P(frompoints, topoints, valid);

    if (valid == false)
        return new InvalidImp;
    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecProjectivityB2Qu[] = {
    {ObjectImp::stype(), kli18n("Generic projective transformation of this object"), kli18n("Select the object to transform..."), false},
    {FilledPolygonImp::stype4(),
     kli18n("Map this quadrilateral"),
     kli18n("Select the quadrilateral that has to be transformed onto a given quadrilateral..."),
     false},
    {FilledPolygonImp::stype4(),
     kli18n("onto this other quadrilateral"),
     kli18n("Select the quadrilateral that is the image by the projective transformation of the first quadrilateral..."),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ProjectivityB2QuType)

ProjectivityB2QuType::ProjectivityB2QuType()
    : ArgsParserObjectType("ProjectivityB2Qu", argsspecProjectivityB2Qu, 3)
{
}

ProjectivityB2QuType::~ProjectivityB2QuType()
{
}

const ProjectivityB2QuType *ProjectivityB2QuType::instance()
{
    static const ProjectivityB2QuType t;
    return &t;
}

ObjectImp *ProjectivityB2QuType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    std::vector<Coordinate> frompoints = static_cast<const FilledPolygonImp *>(args[1])->points();
    std::vector<Coordinate> topoints = static_cast<const FilledPolygonImp *>(args[2])->points();

    bool valid = true;
    Transformation t = Transformation::projectivityGI4P(frompoints, topoints, valid);

    if (valid == false)
        return new InvalidImp;
    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecProjectivityGI4P[] = {
    {ObjectImp::stype(), kli18n("Generic projective transformation of this object"), kli18n("Select the object to transform..."), false},
    {PointImp::stype(),
     kli18n("First of 4 starting points"),
     kli18n("Select the first of the four starting points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Second of 4 starting points"),
     kli18n("Select the second of the four starting points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Third of 4 starting points"),
     kli18n("Select the third of the four starting points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Fourth of 4 starting points"),
     kli18n("Select the fourth of the four starting points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Transformed position of first point"),
     kli18n("Select the first of the four end points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Transformed position of second point"),
     kli18n("Select the second of the four end points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Transformed position of third point"),
     kli18n("Select the third of the four end points of the generic projectivity..."),
     false},
    {PointImp::stype(),
     kli18n("Transformed position of fourth point"),
     kli18n("Select the fourth of the four end points of the generic projectivity..."),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ProjectivityGI4PType)

ProjectivityGI4PType::ProjectivityGI4PType()
    : ArgsParserObjectType("ProjectivityGI4P", argsspecProjectivityGI4P, 9)
{
}

ProjectivityGI4PType::~ProjectivityGI4PType()
{
}

const ProjectivityGI4PType *ProjectivityGI4PType::instance()
{
    static const ProjectivityGI4PType t;
    return &t;
}

ObjectImp *ProjectivityGI4PType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    std::vector<Coordinate> frompoints;
    std::vector<Coordinate> topoints;
    for (uint i = 0; i < 4; ++i) {
        frompoints.push_back(static_cast<const PointImp *>(args[i + 1])->coordinate());
        topoints.push_back(static_cast<const PointImp *>(args[i + 5])->coordinate());
    }

    bool valid = true;
    Transformation t = Transformation::projectivityGI4P(frompoints, topoints, valid);

    if (valid == false)
        return new InvalidImp;
    return args[0]->transform(t);
}

static const ArgsParser::spec argsspecCastShadow[] = {
    {ObjectImp::stype(), kli18n("Cast the shadow of this object"), kli18n("Select the object of which you want to construct the shadow..."), false},
    {PointImp::stype(),
     kli18n("Cast a shadow from this light source"),
     kli18n("Select the light source from which the shadow should originate..."),
     false},
    {AbstractLineImp::stype(), kli18n("Cast a shadow on the horizon represented by this line"), kli18n("Select the horizon for the shadow..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(CastShadowType)

CastShadowType::CastShadowType()
    : ArgsParserObjectType("CastShadow", argsspecCastShadow, 3)
{
}

CastShadowType::~CastShadowType()
{
}

const CastShadowType *CastShadowType::instance()
{
    static const CastShadowType t;
    return &t;
}

ObjectImp *CastShadowType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate lightsrc = static_cast<const PointImp *>(args[1])->coordinate();
    LineData d = static_cast<const AbstractLineImp *>(args[2])->data();
    return args[0]->transform(Transformation::castShadow(lightsrc, d));
}

const ObjectImpType *TranslatedType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *PointReflectionType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *LineReflectionType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *RotationType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ScalingOverCenterType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ScalingOverCenter2Type::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ScalingOverLineType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ScalingOverLine2Type::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ProjectiveRotationType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *HarmonicHomologyType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *AffinityB2TrType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *AffinityGI3PType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ProjectivityB2QuType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *ProjectivityGI4PType::resultId() const
{
    return ObjectImp::stype();
}

const ObjectImpType *CastShadowType::resultId() const
{
    return ObjectImp::stype();
}

bool TranslatedType::isTransform() const
{
    return true;
}

bool PointReflectionType::isTransform() const
{
    return true;
}

bool LineReflectionType::isTransform() const
{
    return true;
}

bool RotationType::isTransform() const
{
    return true;
}

bool ScalingOverCenterType::isTransform() const
{
    return true;
}

bool ScalingOverCenter2Type::isTransform() const
{
    return true;
}

bool ScalingOverLineType::isTransform() const
{
    return true;
}

bool ScalingOverLine2Type::isTransform() const
{
    return true;
}

bool ProjectiveRotationType::isTransform() const
{
    return true;
}

bool HarmonicHomologyType::isTransform() const
{
    return true;
}

bool AffinityB2TrType::isTransform() const
{
    return true;
}

bool AffinityGI3PType::isTransform() const
{
    return true;
}

bool ProjectivityB2QuType::isTransform() const
{
    return true;
}

bool ProjectivityGI4PType::isTransform() const
{
    return true;
}

bool CastShadowType::isTransform() const
{
    return true;
}

static const ArgsParser::spec argsspecApplyTransformation[] = {
    {ObjectImp::stype(), kli18n("Transform this object"), {}, false},
    {TransformationImp::stype(), kli18n("Transform using this transformation"), {}, false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ApplyTransformationObjectType)

ApplyTransformationObjectType::ApplyTransformationObjectType()
    : ArgsParserObjectType("ApplyTransformation", argsspecApplyTransformation, 2)
{
}

ApplyTransformationObjectType::~ApplyTransformationObjectType()
{
}

const ApplyTransformationObjectType *ApplyTransformationObjectType::instance()
{
    static const ApplyTransformationObjectType t;
    return &t;
}

ObjectImp *ApplyTransformationObjectType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;
    return args[0]->transform(static_cast<const TransformationImp *>(args[1])->data());
}

const ObjectImpType *ApplyTransformationObjectType::resultId() const
{
    return ObjectImp::stype();
}

bool ApplyTransformationObjectType::isTransform() const
{
    return true;
}

bool SimilitudeType::isTransform() const
{
    return true;
}

const ObjectImpType *SimilitudeType::resultId() const
{
    return ObjectImp::stype();
}

const SimilitudeType *SimilitudeType::instance()
{
    static const SimilitudeType t;
    return &t;
}

ObjectImp *SimilitudeType::calc(const Args &args, const KigDocument &) const
{
    if (!margsparser.checkArgs(args))
        return new InvalidImp;

    Coordinate c = static_cast<const PointImp *>(args[1])->coordinate();
    Coordinate a = static_cast<const PointImp *>(args[2])->coordinate();
    Coordinate b = static_cast<const PointImp *>(args[3])->coordinate();
    a -= c;
    b -= c;
    double factor = sqrt(b.squareLength() / a.squareLength());
    double theta = atan2(b.y, b.x) - atan2(a.y, a.x);

    return args[0]->transform(Transformation::similitude(c, theta, factor));
}

SimilitudeType::~SimilitudeType()
{
}

static const ArgsParser::spec argsspecSimilitude[] = {
    {ObjectImp::stype(), kli18n("Apply a similitude to this object"), kli18n("Select the object to transform..."), false},
    {PointImp::stype(), kli18n("Apply a similitude with this center"), kli18n("Select the center for the similitude..."), false},
    {PointImp::stype(),
     kli18n("Apply a similitude mapping this point onto another point"),
     kli18n("Select the point which the similitude should map onto another point..."),
     false},
    {PointImp::stype(),
     kli18n("Apply a similitude mapping a point onto this point"),
     kli18n("Select the point onto which the similitude should map the first point..."),
     false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(SimilitudeType)

SimilitudeType::SimilitudeType()
    : ArgsParserObjectType("Similitude", argsspecSimilitude, 4)
{
}
