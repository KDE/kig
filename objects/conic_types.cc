// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "conic_types.h"

#include "../kig/kig_commands.h"
#include "../kig/kig_part.h"
#include "../misc/common.h"
#include "../misc/conic-common.h"
#include "bogus_imp.h"
#include "circle_imp.h"
#include "conic_imp.h"
#include "line_imp.h"
#include "object_calcer.h"
#include "point_imp.h"

static const KLazyLocalizedString conic_constructstatement = kli18n("Construct a conic through this point");

static const struct ArgsParser::spec argsspecConicB5P[] = {
    {PointImp::stype(), conic_constructstatement, kli18n("Select a point for the new conic to go through..."), true},
    {PointImp::stype(), conic_constructstatement, kli18n("Select a point for the new conic to go through..."), true},
    {PointImp::stype(), conic_constructstatement, kli18n("Select a point for the new conic to go through..."), true},
    {PointImp::stype(), conic_constructstatement, kli18n("Select a point for the new conic to go through..."), true},
    {PointImp::stype(), conic_constructstatement, kli18n("Select a point for the new conic to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicB5PType)

ConicB5PType::ConicB5PType()
    : ArgsParserObjectType("ConicB5P", argsspecConicB5P, 5)
{
}

ConicB5PType::~ConicB5PType()
{
}

ObjectImp *ConicB5PType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents, 1))
        return new InvalidImp;
    std::vector<Coordinate> points;

    for (Args::const_iterator i = parents.begin(); i != parents.end(); ++i)
        points.push_back(static_cast<const PointImp *>(*i)->coordinate());

    ConicCartesianData d = calcConicThroughPoints(points, zerotilt, parabolaifzt, ysymmetry);
    if (d.valid())
        return new ConicImpCart(d);
    else
        return new InvalidImp;
}

const ConicB5PType *ConicB5PType::instance()
{
    static const ConicB5PType t;
    return &t;
}

static const ArgsParser::spec argsspecConicBAAP[] = {
    {AbstractLineImp::stype(), kli18n("Construct a conic with this asymptote"), kli18n("Select the first asymptote of the new conic..."), false},
    {AbstractLineImp::stype(), kli18n("Construct a conic with this asymptote"), kli18n("Select the second asymptote of the new conic..."), false},
    {PointImp::stype(), kli18n("Construct a conic through this point"), kli18n("Select a point for the new conic to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicBAAPType)

ConicBAAPType::ConicBAAPType()
    : ArgsParserObjectType("ConicBAAP", argsspecConicBAAP, 3)
{
}

ConicBAAPType::~ConicBAAPType()
{
}

const ConicBAAPType *ConicBAAPType::instance()
{
    static const ConicBAAPType t;
    return &t;
}

ObjectImp *ConicBAAPType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents))
        return new InvalidImp;
    const LineData la = static_cast<const AbstractLineImp *>(parents[0])->data();
    const LineData lb = static_cast<const AbstractLineImp *>(parents[1])->data();
    const Coordinate c = static_cast<const PointImp *>(parents[2])->coordinate();

    return new ConicImpCart(calcConicByAsymptotes(la, lb, c));
}

ObjectImp *ConicBFFPType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents, 2))
        return new InvalidImp;
    std::vector<Coordinate> cs;

    for (Args::const_iterator i = parents.begin(); i != parents.end(); ++i)
        cs.push_back(static_cast<const PointImp *>(*i)->coordinate());

    return new ConicImpPolar(calcConicBFFP(cs, type()));
}

ConicBFFPType::ConicBFFPType(const char *fullname, const ArgsParser::spec *spec, int n)
    : ArgsParserObjectType(fullname, spec, n)
{
}

ConicBFFPType::~ConicBFFPType()
{
}

static const KLazyLocalizedString constructellipsewithfocusstat = kli18n("Construct an ellipse with this focus");

static const ArgsParser::spec argsspecEllipseBFFP[] = {
    {PointImp::stype(), constructellipsewithfocusstat, kli18n("Select the first focus of the new ellipse..."), false},
    {PointImp::stype(), constructellipsewithfocusstat, kli18n("Select the second focus of the new ellipse..."), false},
    {PointImp::stype(), kli18n("Construct an ellipse through this point"), kli18n("Select a point for the new ellipse to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(EllipseBFFPType)

EllipseBFFPType::EllipseBFFPType()
    : ConicBFFPType("EllipseBFFP", argsspecEllipseBFFP, 3)
{
}

EllipseBFFPType::~EllipseBFFPType()
{
}

int EllipseBFFPType::type() const
{
    return 1;
}

const EllipseBFFPType *EllipseBFFPType::instance()
{
    static const EllipseBFFPType t;
    return &t;
}

static const KLazyLocalizedString constructhyperbolawithfocusstat = kli18n("Construct a hyperbola with this focus");

static const ArgsParser::spec argsspecHyperbolaBFFP[] = {
    {PointImp::stype(), constructhyperbolawithfocusstat, kli18n("Select the first focus of the new hyperbola..."), false},
    {PointImp::stype(), constructhyperbolawithfocusstat, kli18n("Select the second focus of the new hyperbola..."), false},
    {PointImp::stype(), kli18n("Construct a hyperbola through this point"), kli18n("Select a point for the new hyperbola to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(HyperbolaBFFPType)

HyperbolaBFFPType::HyperbolaBFFPType()
    : ConicBFFPType("HyperbolaBFFP", argsspecHyperbolaBFFP, 3)
{
}

HyperbolaBFFPType::~HyperbolaBFFPType()
{
}

const HyperbolaBFFPType *HyperbolaBFFPType::instance()
{
    static const HyperbolaBFFPType t;
    return &t;
}

int HyperbolaBFFPType::type() const
{
    return -1;
}

const ConicBDFPType *ConicBDFPType::instance()
{
    static const ConicBDFPType t;
    return &t;
}

static const struct ArgsParser::spec argsspecConicBDFP[] = {
    {AbstractLineImp::stype(), kli18n("Construct a conic with this line as directrix"), kli18n("Select the directrix of the new conic..."), false},
    {PointImp::stype(), kli18n("Construct a conic with this point as focus"), kli18n("Select the focus of the new conic..."), false},
    {PointImp::stype(), kli18n("Construct a conic through this point"), kli18n("Select a point for the new conic to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicBDFPType)

ConicBDFPType::ConicBDFPType()
    : ArgsParserObjectType("ConicBDFP", argsspecConicBDFP, 3)
{
}

ConicBDFPType::~ConicBDFPType()
{
}

ObjectImp *ConicBDFPType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents, 2))
        return new InvalidImp;

    const LineData line = static_cast<const AbstractLineImp *>(parents[0])->data();
    const Coordinate focus = static_cast<const PointImp *>(parents[1])->coordinate();

    Coordinate point;
    if (parents.size() == 3)
        point = static_cast<const PointImp *>(parents[2])->coordinate();
    else {
        /* !!!! costruisci point come punto medio dell'altezza tra fuoco e d. */
        Coordinate ba = line.dir();
        Coordinate fa = focus - line.b;
        double balsq = ba.x * ba.x + ba.y * ba.y;
        double scal = (fa.x * ba.x + fa.y * ba.y) / balsq;
        point = 0.5 * (line.a + focus + scal * ba);
    };
    return new ConicImpPolar(calcConicBDFP(line, focus, point));
}

static const KLazyLocalizedString constructparabolathroughpointstat = kli18n("Construct a parabola through this point");

static const ArgsParser::spec argsspecParabolaBTP[] = {
    {PointImp::stype(), constructparabolathroughpointstat, kli18n("Select a point for the new parabola to go through..."), true},
    {PointImp::stype(), constructparabolathroughpointstat, kli18n("Select a point for the new parabola to go through..."), true},
    {PointImp::stype(), constructparabolathroughpointstat, kli18n("Select a point for the new parabola to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ParabolaBTPType)

ParabolaBTPType::ParabolaBTPType()
    : ArgsParserObjectType("ParabolaBTP", argsspecParabolaBTP, 3)
{
}

ParabolaBTPType::~ParabolaBTPType()
{
}

const ParabolaBTPType *ParabolaBTPType::instance()
{
    static const ParabolaBTPType t;
    return &t;
}

ObjectImp *ParabolaBTPType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents, 2))
        return new InvalidImp;

    std::vector<Coordinate> points;
    for (Args::const_iterator i = parents.begin(); i != parents.end(); ++i)
        points.push_back(static_cast<const PointImp *>(*i)->coordinate());

    ConicCartesianData d = calcConicThroughPoints(points, zerotilt, parabolaifzt, ysymmetry);
    if (d.valid())
        return new ConicImpCart(d);
    else
        return new InvalidImp;
}

static const ArgsParser::spec argsspecConicPolarPoint[] = {{ConicImp::stype(),
                                                            kli18n("Construct a polar point wrt. this conic"),
                                                            kli18n("Select the conic wrt. which you want to construct a polar point..."),
                                                            false},
                                                           {AbstractLineImp::stype(),
                                                            kli18n("Construct the polar point of this line"),
                                                            kli18n("Select the line of which you want to construct the polar point..."),
                                                            false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicPolarPointType)

ConicPolarPointType::ConicPolarPointType()
    : ArgsParserObjectType("ConicPolarPoint", argsspecConicPolarPoint, 2)
{
}

ConicPolarPointType::~ConicPolarPointType()
{
}

const ConicPolarPointType *ConicPolarPointType::instance()
{
    static const ConicPolarPointType t;
    return &t;
}

ObjectImp *ConicPolarPointType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents))
        return new InvalidImp;

    const ConicCartesianData c = static_cast<const ConicImp *>(parents[0])->cartesianData();
    const LineData l = static_cast<const AbstractLineImp *>(parents[1])->data();
    const Coordinate p = calcConicPolarPoint(c, l);
    if (p.valid())
        return new PointImp(p);
    else
        return new InvalidImp;
}

static const ArgsParser::spec argsspecConicPolarLine[] = {{ConicImp::stype(),
                                                           kli18n("Construct a polar line wrt. this conic"),
                                                           kli18n("Select the conic wrt. which you want to construct a polar point..."),
                                                           false},
                                                          {PointImp::stype(),
                                                           kli18n("Construct the polar line of this point"),
                                                           kli18n("Select the line of which you want to construct the polar point..."),
                                                           false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicPolarLineType)

ConicPolarLineType::ConicPolarLineType()
    : ArgsParserObjectType("ConicPolarLine", argsspecConicPolarLine, 2)
{
}

ConicPolarLineType::~ConicPolarLineType()
{
}

const ConicPolarLineType *ConicPolarLineType::instance()
{
    static const ConicPolarLineType t;
    return &t;
}

ObjectImp *ConicPolarLineType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents))
        return new InvalidImp;

    const ConicCartesianData c = static_cast<const ConicImp *>(parents[0])->cartesianData();
    const Coordinate p = static_cast<const PointImp *>(parents[1])->coordinate();
    bool valid = true;
    const LineData l = calcConicPolarLine(c, p, valid);
    if (valid)
        return new LineImp(l);
    else
        return new InvalidImp;
}

static const ArgsParser::spec argsspecConicDirectrix[] = {{ConicImp::stype(),
                                                           kli18n("Construct the directrix of this conic"),
                                                           kli18n("Select the conic of which you want to construct the directrix..."),
                                                           false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicDirectrixType)

ConicDirectrixType::ConicDirectrixType()
    : ArgsParserObjectType("ConicDirectrix", argsspecConicDirectrix, 1)
{
}

ConicDirectrixType::~ConicDirectrixType()
{
}

const ConicDirectrixType *ConicDirectrixType::instance()
{
    static const ConicDirectrixType t;
    return &t;
}

ObjectImp *ConicDirectrixType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents))
        return new InvalidImp;

    const ConicPolarData data = static_cast<const ConicImp *>(parents[0])->polarData();

    double ec = data.ecostheta0;
    double es = data.esintheta0;
    double eccsq = ec * ec + es * es;

    Coordinate a = data.focus1 - data.pdimen / eccsq * Coordinate(ec, es);
    Coordinate b = a + Coordinate(-es, ec);
    return new LineImp(a, b);
}

static const KLazyLocalizedString hyperbolatpstatement = kli18n("Construct a hyperbola through this point");

static const ArgsParser::spec argsspecHyperbolaB4P[] = {
    {PointImp::stype(), hyperbolatpstatement, kli18n("Select a point for the new hyperbola to go through..."), true},
    {PointImp::stype(), hyperbolatpstatement, kli18n("Select a point for the new hyperbola to go through..."), true},
    {PointImp::stype(), hyperbolatpstatement, kli18n("Select a point for the new hyperbola to go through..."), true},
    {PointImp::stype(), hyperbolatpstatement, kli18n("Select a point for the new hyperbola to go through..."), true}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(EquilateralHyperbolaB4PType)

EquilateralHyperbolaB4PType::EquilateralHyperbolaB4PType()
    : ArgsParserObjectType("EquilateralHyperbolaB4P", argsspecHyperbolaB4P, 4)
{
}

EquilateralHyperbolaB4PType::~EquilateralHyperbolaB4PType()
{
}

const EquilateralHyperbolaB4PType *EquilateralHyperbolaB4PType::instance()
{
    static const EquilateralHyperbolaB4PType t;
    return &t;
}

ObjectImp *EquilateralHyperbolaB4PType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents, 1))
        return new InvalidImp;

    std::vector<Coordinate> pts;
    for (Args::const_iterator i = parents.begin(); i != parents.end(); ++i)
        pts.push_back(static_cast<const PointImp *>(*i)->coordinate());

    ConicCartesianData d = calcConicThroughPoints(pts, equilateral);
    if (d.valid())
        return new ConicImpCart(d);
    else
        return new InvalidImp;
}

static const ArgsParser::spec argsspecParabolaBDP[] = {
    {AbstractLineImp::stype(), kli18n("Construct a parabola with this directrix"), kli18n("Select the directrix of the new parabola..."), false},
    {PointImp::stype(), kli18n("Construct a parabola with this focus"), kli18n("Select the focus of the new parabola..."), false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ParabolaBDPType)

ParabolaBDPType::ParabolaBDPType()
    : ObjectLPType("ParabolaBDP", argsspecParabolaBDP, 2)
{
}

ParabolaBDPType::~ParabolaBDPType()
{
}

const ParabolaBDPType *ParabolaBDPType::instance()
{
    static const ParabolaBDPType t;
    return &t;
}

ObjectImp *ParabolaBDPType::calc(const LineData &l, const Coordinate &c) const
{
    ConicPolarData ret;
    Coordinate ldir = l.dir();
    ldir = ldir.normalize();
    ret.focus1 = c;
    ret.ecostheta0 = -ldir.y;
    ret.esintheta0 = ldir.x;
    Coordinate fa = c - l.a;
    ret.pdimen = fa.y * ldir.x - fa.x * ldir.y;
    ConicImpPolar *r = new ConicImpPolar(ret);
    qDebug() << r->conicTypeString();
    return r;
}

static const ArgsParser::spec argsspecConicAsymptote[] = {{ConicImp::stype(),
                                                           kli18n("Construct the asymptotes of this conic"),
                                                           kli18n("Select the conic of which you want to construct the asymptotes..."),
                                                           false},
                                                          {IntImp::stype(), "param", {}, false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicAsymptoteType)

ConicAsymptoteType::ConicAsymptoteType()
    : ArgsParserObjectType("ConicAsymptote", argsspecConicAsymptote, 2)
{
}

ConicAsymptoteType::~ConicAsymptoteType()
{
}

const ConicAsymptoteType *ConicAsymptoteType::instance()
{
    static const ConicAsymptoteType t;
    return &t;
}

ObjectImp *ConicAsymptoteType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents))
        return new InvalidImp;

    bool valid = true;
    const LineData ret = calcConicAsymptote(static_cast<const ConicImp *>(parents[0])->cartesianData(), static_cast<const IntImp *>(parents[1])->data(), valid);

    if (valid)
        return new LineImp(ret);
    else
        return new InvalidImp;
}

static const KLazyLocalizedString radicallinesstatement = kli18n("Construct the radical lines of this conic");

static const ArgsParser::spec argsspecConicRadical[] = {
    {ConicImp::stype(), radicallinesstatement, kli18n("Select the first of the two conics of which you want to construct the radical line..."), false},
    {ConicImp::stype(), radicallinesstatement, kli18n("Select the other of the two conic of which you want to construct the radical line..."), false},
    {IntImp::stype(), "param", {}, false},
    {IntImp::stype(), "param", {}, false}};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE(ConicRadicalType)

ConicRadicalType::ConicRadicalType()
    : ArgsParserObjectType("ConicRadical", argsspecConicRadical, 4)
{
}

const ConicRadicalType *ConicRadicalType::instance()
{
    static const ConicRadicalType t;
    return &t;
}

ObjectImp *ConicRadicalType::calc(const Args &parents, const KigDocument &) const
{
    if (!margsparser.checkArgs(parents))
        return new InvalidImp;
    if (parents[0]->inherits(CircleImp::stype()) && parents[1]->inherits(CircleImp::stype())) {
        if (static_cast<const IntImp *>(parents[2])->data() != 1)
            return new InvalidImp;
        else {
            const CircleImp *c1 = static_cast<const CircleImp *>(parents[0]);
            const CircleImp *c2 = static_cast<const CircleImp *>(parents[1]);
            const Coordinate a = calcCircleRadicalStartPoint(c1->center(), c2->center(), c1->squareRadius(), c2->squareRadius());
            return new LineImp(a, calcPointOnPerpend(LineData(c1->center(), c2->center()), a));
        };
    } else {
        bool valid = true;
        const LineData l = calcConicRadical(static_cast<const ConicImp *>(parents[0])->cartesianData(),
                                            static_cast<const ConicImp *>(parents[1])->cartesianData(),
                                            static_cast<const IntImp *>(parents[2])->data(),
                                            static_cast<const IntImp *>(parents[3])->data(),
                                            valid);
        if (valid)
            return new LineImp(l);
        else
            return new InvalidImp;
    };
}

ConicRadicalType::~ConicRadicalType()
{
}

const ObjectImpType *ConicB5PType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *ConicBAAPType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *ConicBFFPType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *ConicBDFPType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *ParabolaBTPType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *EquilateralHyperbolaB4PType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *ConicPolarPointType::resultId() const
{
    return PointImp::stype();
}

const ObjectImpType *ConicPolarLineType::resultId() const
{
    return LineImp::stype();
}

const ObjectImpType *ConicDirectrixType::resultId() const
{
    return LineImp::stype();
}

const ObjectImpType *ParabolaBDPType::resultId() const
{
    return ConicImp::stype();
}

const ObjectImpType *ConicAsymptoteType::resultId() const
{
    return LineImp::stype();
}

const ObjectImpType *ConicRadicalType::resultId() const
{
    return LineImp::stype();
}

QStringList ConicRadicalType::specialActions() const
{
    QStringList ret;
    ret << i18n("Switch Radical Lines");
    return ret;
}

void ConicRadicalType::executeAction(int i, ObjectHolder &, ObjectTypeCalcer &t, KigPart &d, KigWidget &, NormalMode &) const
{
    assert(i == 0);
    std::vector<ObjectCalcer *> parents = t.parents();
    assert(dynamic_cast<ObjectConstCalcer *>(parents[3]));
    ObjectConstCalcer *zeroindexo = static_cast<ObjectConstCalcer *>(parents[3]);
    MonitorDataObjects mon(zeroindexo);
    assert(zeroindexo->imp()->inherits(IntImp::stype()));
    int oldzeroindex = static_cast<const IntImp *>(zeroindexo->imp())->data();
    int newzeroindex = oldzeroindex % 3 + 1;
    zeroindexo->setImp(new IntImp(newzeroindex));
    KigCommand *kc = new KigCommand(d, i18n("Switch Conic Radical Lines"));
    mon.finish(kc);
    d.history()->push(kc);
}
