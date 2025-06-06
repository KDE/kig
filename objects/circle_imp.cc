// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "circle_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"

#include "../misc/coordinate_system.h"
#include "../misc/equationstring.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <KLazyLocalizedString>
#include <math.h>

CircleImp::CircleImp(const Coordinate &center, double radius)
    : mcenter(center)
    , mradius(radius)
{
}

CircleImp::~CircleImp()
{
}

ObjectImp *CircleImp::transform(const Transformation &t) const
{
    if (t.isHomothetic()) {
        Coordinate nc = t.apply(mcenter);
        double nr = t.apply(mradius);
        if (nc.valid())
            return new CircleImp(nc, nr);
        else
            return new InvalidImp;
    } else {
        // domi: i should return a ConicImp here, but i don't know how to
        // calculate it..
        return Parent::transform(t);
    };
}

void CircleImp::draw(KigPainter &p) const
{
    p.drawCircle(mcenter, fabs(mradius));
}

bool CircleImp::contains(const Coordinate &p, int width, const KigWidget &w) const
{
    return fabs((mcenter - p).length() - fabs(mradius)) <= w.screenInfo().normalMiss(width);
}

bool CircleImp::inRect(const Rect &r, int width, const KigWidget &w) const
{
    // first we check if the rect contains at least one of the
    // north/south/east/west points of the circle
    if (r.contains(mcenter + Coordinate(0, -mradius)))
        return true;
    if (r.contains(mcenter + Coordinate(mradius, 0)))
        return true;
    if (r.contains(mcenter + Coordinate(0, mradius)))
        return true;
    if (r.contains(mcenter + Coordinate(-mradius, 0)))
        return true;

    // we allow a miss of some pixels ..
    double miss = w.screenInfo().normalMiss(width);
    double bigradius = fabs(mradius) + miss;
    bigradius *= bigradius;
    double smallradius = fabs(mradius) - miss;
    smallradius *= smallradius;

    const int in = -1;
    const int undecided = 0;
    const int out = 1;

    int inorout = undecided;

    Coordinate coords[4];
    coords[0] = r.topLeft();
    coords[1] = r.topRight();
    coords[2] = r.bottomRight();
    coords[3] = r.bottomLeft();

    // we check if the corners of the rect are either
    for (Coordinate *i = coords; i < coords + 4; ++i) {
        double t = (*i - mcenter).squareLength();
        if (t >= bigradius) {
            if (inorout == in)
                return true;
            inorout = out;
        } else if (t <= smallradius) {
            if (inorout == out)
                return true;
            inorout = in;
        }
    }
    return inorout == undecided;
}

bool CircleImp::valid() const
{
    return true;
}

int CircleImp::numberOfProperties() const
{
    // We _intentionally_ do not use the Conic properties..
    return CurveImp::numberOfProperties() + 7;
}

const QByteArrayList CircleImp::propertiesInternalNames() const
{
    QByteArrayList l = CurveImp::propertiesInternalNames();
    l << "surface";
    l << "circumference";
    l << "radius";
    l << "center";
    l << "cartesian-equation";
    l << "simply-cartesian-equation";
    l << "polar-equation";
    assert(l.size() == CircleImp::numberOfProperties());
    return l;
}

const QList<KLazyLocalizedString> CircleImp::properties() const
{
    QList<KLazyLocalizedString> l = CurveImp::properties();
    l << kli18n("Surface");
    l << kli18n("Circumference");
    l << kli18n("Radius");
    l << kli18n("Center");
    l << kli18n("Expanded Cartesian Equation");
    l << kli18n("Cartesian Equation");
    l << kli18n("Polar Equation");
    assert(l.size() == CircleImp::numberOfProperties());
    return l;
}

const ObjectImpType *CircleImp::impRequirementForProperty(int which) const
{
    if (which < CurveImp::numberOfProperties())
        return CurveImp::impRequirementForProperty(which);
    else
        return CircleImp::stype();
}

const char *CircleImp::iconForProperty(int which) const
{
    assert(which < CircleImp::numberOfProperties());
    if (which < CurveImp::numberOfProperties())
        return CurveImp::iconForProperty(which);
    else if (which == CurveImp::numberOfProperties())
        return "areaCircle"; // surface
    else if (which == CurveImp::numberOfProperties() + 1)
        return "circumference"; // circumference
    else if (which == CurveImp::numberOfProperties() + 2)
        return ""; // radius
    else if (which == CurveImp::numberOfProperties() + 3)
        return "baseCircle"; // circle center
    else if (which == CurveImp::numberOfProperties() + 4)
        return "kig_text"; // cartesian equation
    else if (which == CurveImp::numberOfProperties() + 5)
        return "kig_text"; // simply cartesian equation
    else if (which == CurveImp::numberOfProperties() + 6)
        return "kig_text"; // polar equation
    else
        assert(false);
    return "";
}

ObjectImp *CircleImp::property(int which, const KigDocument &w) const
{
    assert(which < CircleImp::numberOfProperties());
    if (which < CurveImp::numberOfProperties())
        return CurveImp::property(which, w);
    if (which == CurveImp::numberOfProperties())
        return new DoubleImp(surface());
    else if (which == CurveImp::numberOfProperties() + 1)
        return new DoubleImp(circumference());
    else if (which == CurveImp::numberOfProperties() + 2)
        return new DoubleImp(radius());
    else if (which == CurveImp::numberOfProperties() + 3)
        return new PointImp(center());
    else if (which == CurveImp::numberOfProperties() + 4)
        return new StringImp(cartesianEquationString(w));
    else if (which == CurveImp::numberOfProperties() + 5)
        return new StringImp(simplyCartesianEquationString(w));
    else if (which == CurveImp::numberOfProperties() + 6)
        return new StringImp(polarEquationString(w));
    else
        assert(false);
    return new InvalidImp;
}

const Coordinate CircleImp::center() const
{
    return mcenter;
}

double CircleImp::radius() const
{
    return fabs(mradius);
}

double CircleImp::orientation() const
{
    return (mradius > 0) ? 1 : (-1);
}

double CircleImp::surface() const
{
    return M_PI * squareRadius();
}

double CircleImp::squareRadius() const
{
    return mradius * mradius;
}

double CircleImp::circumference() const
{
    return 2 * M_PI * radius();
}

QString CircleImp::polarEquationString(const KigDocument &w) const
{
    ConicPolarData data = polarData();
    QString ret = ki18n("rho = %1   [centered at %2]").subs(data.pdimen, 0, 'g', 3).subs(w.coordinateSystem().fromScreen(data.focus1, w)).toString();
    return ret;
}

QString CircleImp::cartesianEquationString(const KigDocument &) const
{
    //  QString ret = i18n( "x² + y² + %1 x + %2 y + %3 = 0" );
    ConicCartesianData data = cartesianData();
    EquationString ret = EquationString(QLatin1String(""));
    bool needsign = false;
    ret.addTerm(1., ret.x2(), needsign);
    ret.addTerm(1., ret.y2(), needsign);
    ret.addTerm(data.coeffs[3], ret.x(), needsign);
    ret.addTerm(data.coeffs[4], ret.y(), needsign);
    ret.addTerm(data.coeffs[5], QLatin1String(""), needsign);
    ret.append(" = 0");
    //  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
    //  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
    //  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
    return ret;
}

QString CircleImp::simplyCartesianEquationString(const KigDocument &) const
{
    EquationString ret = EquationString(QStringLiteral("( x"));
    bool needsign = true;
    ret.addTerm(-mcenter.x, QLatin1String(""), needsign);
    ret.append(QStringLiteral(" )² + ( y"));
    ret.addTerm(-mcenter.y, QLatin1String(""), needsign);
    ret.append(QStringLiteral(" )² = "));
    needsign = false;
    ret.addTerm(mradius * mradius, QLatin1String(""), needsign);
    ret.prettify();

    //  QString ret = i18n( "( x - %1 )² + ( y - %2 )² = %3" );
    //  ret = ret.arg( mcenter.x, 0, 'g', 3 );
    //  ret = ret.arg( mcenter.y, 0, 'g', 3 );
    //  ret = ret.arg( mradius * mradius, 0, 'g', 3 );
    return ret;
}

Coordinate CircleImp::focus1() const
{
    return center();
}

Coordinate CircleImp::focus2() const
{
    return center();
}

int CircleImp::conicType() const
{
    return 1;
}

const ConicCartesianData CircleImp::cartesianData() const
{
    Coordinate c = center();
    double sqr = squareRadius();
    ConicCartesianData data(1.0, 1.0, 0.0, -2 * c.x, -2 * c.y, c.x * c.x + c.y * c.y - sqr);
    return data;
}

const ConicPolarData CircleImp::polarData() const
{
    return ConicPolarData(center(), radius(), 0, 0);
}

CircleImp *CircleImp::copy() const
{
    return new CircleImp(mcenter, mradius);
}

double CircleImp::getParam(const Coordinate &point, const KigDocument &) const
{
    Coordinate tmp = point - mcenter;
    double ret = atan2(tmp.y, tmp.x) / (2 * M_PI);
    if (ret > 0)
        return ret;
    else
        return ret + 1;
}

const Coordinate CircleImp::getPoint(double p, const KigDocument &) const
{
    return mcenter + Coordinate(cos(p * 2 * M_PI), sin(p * 2 * M_PI)) * mradius;
}

void CircleImp::visit(ObjectImpVisitor *vtor) const
{
    vtor->visit(this);
}

bool CircleImp::equals(const ObjectImp &rhs) const
{
    return rhs.inherits(CircleImp::stype()) && static_cast<const CircleImp &>(rhs).center() == center()
        && static_cast<const CircleImp &>(rhs).radius() == radius();
}

const ObjectImpType *CircleImp::stype()
{
    static const ObjectImpType t(Parent::stype(),
                                 "circle",
                                 kli18n("circle"),
                                 kli18n("Select this circle"),
                                 kli18n("Select circle %1"),
                                 kli18n("Remove a Circle"),
                                 kli18n("Add a Circle"),
                                 kli18n("Move a Circle"),
                                 kli18n("Attach to this circle"),
                                 kli18n("Show a Circle"),
                                 kli18n("Hide a Circle"));
    return &t;
}

const ObjectImpType *CircleImp::type() const
{
    return CircleImp::stype();
}

bool CircleImp::isPropertyDefinedOnOrThroughThisImp(int which) const
{
    assert(which < CircleImp::numberOfProperties());
    if (which < CurveImp::numberOfProperties())
        return CurveImp::isPropertyDefinedOnOrThroughThisImp(which);
    return false;
}

Rect CircleImp::surroundingRect() const
{
    Coordinate d(fabs(mradius), fabs(mradius));
    return Rect(mcenter - d, mcenter + d);
}
