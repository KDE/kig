// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CONIC_IMP_H
#define KIG_OBJECTS_CONIC_IMP_H

#include "curve_imp.h"

#include "../misc/conic-common.h"

/**
 * An ObjectImp representing a conic.
 *
 * A conic is a general second degree curve, and some beautiful theory
 * has been developed about it.  See a math book for more
 * information.  This class is in fact an abstract base class hiding
 * the fact that a ConicImp can be constructed in two ways.  If only
 * its Cartesian equation is known, then you should use ConicImpCart,
 * otherwise, you should use ConicImpPolar.  If the other
 * representation is needed, it will be calculated, but a cartesian
 * representation is rarely needed, and not calculating saves some CPU
 * cycles.
 */
class ConicImp : public CurveImp
{
protected:
    ConicImp();
    ~ConicImp();

public:
    typedef CurveImp Parent;
    /**
     * Returns the ObjectImpType representing the ConicImp type.
     */
    static const ObjectImpType *stype();

    ObjectImp *transform(const Transformation &) const override;

    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &) const override;
    bool inRect(const Rect &r, int width, const KigWidget &) const override;
    bool valid() const;
    Rect surroundingRect() const override;

    int numberOfProperties() const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    const char *iconForProperty(int which) const override;
    ObjectImp *property(int which, const KigDocument &w) const override;

    double getParam(const Coordinate &point, const KigDocument &) const override;
    const Coordinate getPoint(double param, const KigDocument &) const override;

    // getPoint and getParam do not really need the KigDocument arg...

    double getParam(const Coordinate &point) const;
    const Coordinate getPoint(double param) const;

    // information about ourselves.  These are all virtual, because a
    // trivial subclass like CircleImp can override these with trivial
    // versions.

    /**
     * Type of conic.
     * Return what type of conic this is:
     * -1 for a hyperbola
     * 0 for a parabola
     * 1 for an ellipse
     */
    virtual int conicType() const;
    /**
     * A string containing "Hyperbola", "Parabola" or "Ellipse".
     */
    virtual QString conicTypeString() const;
    /**
     * A string containing the cartesian equation of the conic.  This
     * will be of the form "a x^2 + b y^2 + c xy + d x + e y + f = 0".
     */
    virtual QString cartesianEquationString(const KigDocument &w) const;
    /**
     * A string containing the polar equation of the conic.  This will
     * be of the form "rho = pdimen/(1 + ect cos( theta ) + est sin(
     * theta ) )\n    [centered at p]"
     */
    virtual QString polarEquationString(const KigDocument &w) const;
    /**
     * Return the cartesian representation of this conic.
     */
    virtual const ConicCartesianData cartesianData() const;
    /**
     * Return the polar representation of this conic.
     */
    virtual const ConicPolarData polarData() const = 0;
    /**
     * Return the center of this conic.
     */
    virtual Coordinate coniccenter() const;
    /**
     * Return the first focus of this conic.
     */
    virtual Coordinate focus1() const;
    /**
     * Return the second focus of this conic.
     */
    virtual Coordinate focus2() const;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool equals(const ObjectImp &rhs) const override;

    bool containsPoint(const Coordinate &p, const KigDocument &doc) const override;
    bool internalContainsPoint(const Coordinate &p, double threshold) const;
    bool isVerticalParabola(ConicCartesianData &data) const;
};

/**
 * An implementation of ConicImp to be used when only the cartesian
 * equation of the conic is known.
 */
class ConicImpCart : public ConicImp
{
protected:
    ConicCartesianData mcartdata;
    ConicPolarData mpolardata;

public:
    explicit ConicImpCart(const ConicCartesianData &data);
    ~ConicImpCart();
    ConicImpCart *copy() const override;

    const ConicCartesianData cartesianData() const override;
    const ConicPolarData polarData() const override;
};

/**
 * An implementation of ConicImp to be used when only the polar
 * equation of the conic is known.
 */
class ConicImpPolar : public ConicImp
{
    ConicPolarData mdata;

public:
    explicit ConicImpPolar(const ConicPolarData &data);
    ~ConicImpPolar();
    ConicImpPolar *copy() const override;

    const ConicPolarData polarData() const override;
};

/**
 * A conic arc, which is given by the cartesian equation and two angles
 */
class ConicArcImp : public ConicImpCart
{
    double msa;
    double ma;

public:
    typedef CurveImp Parent;
    /**
     * Returns the ObjectImpType representing the ConicImp type.
     */
    static const ObjectImpType *stype();
    /**
     * Construct a Conic Arc with given cartesian equation, start angle and
     * dimension (both in radians).
     */
    ConicArcImp(const ConicCartesianData &data, const double startangle, const double angle);
    ~ConicArcImp();

    ConicArcImp *copy() const override;

    ObjectImp *transform(const Transformation &t) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &) const override;
    bool containsPoint(const Coordinate &p, const KigDocument &doc) const override;
    bool internalContainsPoint(const Coordinate &p, double threshold, const KigDocument &doc) const;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &w) const override;
    const char *iconForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    double getParam(const Coordinate &point, const KigDocument &) const override;
    const Coordinate getPoint(double param, const KigDocument &) const override;

    double getParam(const Coordinate &point) const;
    const Coordinate getPoint(double param) const;

    /**
     * Set the start angle in radians of this arc.
     */
    void setStartAngle(double sa)
    {
        msa = sa;
    }
    /**
     * Set the dimension in radians of this arc.
     */
    void setAngle(double a)
    {
        ma = a;
    }
    /**
     * Return the start point of this arc.
     */
    Coordinate firstEndPoint() const;
    /**
     * Return the end point of this arc.
     */
    Coordinate secondEndPoint() const;

    const ObjectImpType *type() const override;
};

#endif
