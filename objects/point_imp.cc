// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "point_imp.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"
#include "../misc/coordinate_system.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"
#include "bogus_imp.h"

#include <KLazyLocalizedString>

PointImp::PointImp(const Coordinate &c)
    : mc(c)
{
}

Coordinate PointImp::attachPoint() const
{
    return mc;
    //  return Coordinate::invalidCoord();
}

void PointImp::draw(KigPainter &p) const
{
    p.drawFatPoint(mc);
}

bool PointImp::contains(const Coordinate &p, int width, const KigWidget &w) const
{
    int twidth = width == -1 ? 5 : width;
    return (p - mc).length() - twidth * w.screenInfo().pixelWidth() < 0;
}

bool PointImp::inRect(const Rect &r, int width, const KigWidget &w) const
{
    double am = w.screenInfo().normalMiss(width);
    return r.contains(mc, am);
}

int PointImp::numberOfProperties() const
{
    return Parent::numberOfProperties() + 3;
}

const QByteArrayList PointImp::propertiesInternalNames() const
{
    QByteArrayList l = Parent::propertiesInternalNames();
    l << "coordinate";
    l << "coordinate-x";
    l << "coordinate-y";
    assert(l.size() == PointImp::numberOfProperties());
    return l;
}

const QList<KLazyLocalizedString> PointImp::properties() const
{
    QList<KLazyLocalizedString> l = Parent::properties();
    l << kli18n("Coordinate");
    l << kli18n("X coordinate");
    l << kli18n("Y coordinate");
    assert(l.size() == PointImp::numberOfProperties());
    return l;
}

const ObjectImpType *PointImp::impRequirementForProperty(int which) const
{
    if (which < Parent::numberOfProperties())
        return Parent::impRequirementForProperty(which);
    else
        return PointImp::stype();
}

const char *PointImp::iconForProperty(int which) const
{
    if (which < Parent::numberOfProperties())
        return Parent::iconForProperty(which);
    if (which == Parent::numberOfProperties())
        return "pointxy"; // coordinate
    if (which == Parent::numberOfProperties() + 1)
        return "pointxy"; // coordinate-x
    if (which == Parent::numberOfProperties() + 2)
        return "pointxy"; // coordinate-y
    else
        assert(false);
    return "";
}

ObjectImp *PointImp::property(int which, const KigDocument &d) const
{
    if (which < Parent::numberOfProperties())
        return Parent::property(which, d);
    if (which == Parent::numberOfProperties())
        return new PointImp(mc);
    if (which == Parent::numberOfProperties() + 1)
        return new DoubleImp(mc.x);
    if (which == Parent::numberOfProperties() + 2)
        return new DoubleImp(mc.y);
    //  else assert( false );
    return new InvalidImp;
}

PointImp::~PointImp()
{
}

PointImp *PointImp::copy() const
{
    return new PointImp(mc);
}

ObjectImp *PointImp::transform(const Transformation &t) const
{
    Coordinate nc = t.apply(mc);
    if (nc.valid())
        return new PointImp(nc);
    else
        return new InvalidImp();
}

void PointImp::setCoordinate(const Coordinate &c)
{
    mc = c;
}

void PointImp::fillInNextEscape(QString &s, const KigDocument &doc) const
{
    s = s.arg(doc.coordinateSystem().fromScreen(mc, doc));
}

void PointImp::visit(ObjectImpVisitor *vtor) const
{
    vtor->visit(this);
}

bool PointImp::equals(const ObjectImp &rhs) const
{
    return rhs.inherits(PointImp::stype()) && static_cast<const PointImp &>(rhs).coordinate() == coordinate();
}

bool PointImp::canFillInNextEscape() const
{
    return true;
}

const ObjectImpType *PointImp::stype()
{
    static const ObjectImpType t(Parent::stype(),
                                 "point",
                                 kli18n("point"),
                                 kli18n("Select this point"),
                                 kli18n("Select point %1"),
                                 kli18n("Remove a Point"),
                                 kli18n("Add a Point"),
                                 kli18n("Move a Point"),
                                 kli18n("Attach to this point"),
                                 kli18n("Show a Point"),
                                 kli18n("Hide a Point"));
    return &t;
}

const ObjectImpType *PointImp::type() const
{
    return PointImp::stype();
}

bool PointImp::isPropertyDefinedOnOrThroughThisImp(int which) const
{
    return Parent::isPropertyDefinedOnOrThroughThisImp(which);
}

Rect PointImp::surroundingRect() const
{
    return Rect(mc, 0., 0.);
}

/*
 */

BogusPointImp::BogusPointImp(const Coordinate &c)
    : PointImp(c)
{
}

BogusPointImp::~BogusPointImp()
{
}

const ObjectImpType *BogusPointImp::stype()
{
    static const ObjectImpType t(nullptr,
                                 "boguspoint",
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString(),
                                 KLazyLocalizedString());
    return &t;
}

const ObjectImpType *BogusPointImp::type() const
{
    return BogusPointImp::stype();
}
