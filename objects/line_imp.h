// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "curve_imp.h"

#include "../misc/common.h"

class LineData;

/**
 * An ObjectImp class that is the base of the line-like ObjectImp's:
 * SegmentImp, LineImp and RayImp.
 */
class AbstractLineImp : public CurveImp
{
protected:
    LineData mdata;
    AbstractLineImp(const LineData &d);
    AbstractLineImp(const Coordinate &a, const Coordinate &b);

public:
    typedef CurveImp Parent;
    /**
     * Returns the ObjectImpType representing the AbstractLineImp
     * type.
     */
    static const ObjectImpType *stype();

    ~AbstractLineImp();

    bool inRect(const Rect &r, int width, const KigWidget &) const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &d) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    /**
     * Get the slope of this AbstractLineImp.  For a line through
     * points a( xa, ya ) and b ( xb, yb ), this means the value ( yb -
     * ya ) / ( xb - xa ).
     */
    double slope() const;
    /**
     * Get a string containing the equation of this line in the form "y
     * = a * x + b ".
     */
    const QString equationString() const;
    /**
     * Get the LineData for this AbstractLineImp.
     */
    LineData data() const;

    bool equals(const ObjectImp &rhs) const override;
};

/**
 * An ObjectImp representing a segment
 */
class SegmentImp : public AbstractLineImp
{
public:
    typedef AbstractLineImp Parent;
    /**
     * Returns the ObjectImpType representing the SegmentImp
     * type.
     */
    static const ObjectImpType *stype();

    /**
     * Construct a new segment from point a to point b.
     */
    SegmentImp(const Coordinate &a, const Coordinate &b);
    /**
     * Construct a new segment from a LineData.
     */
    explicit SegmentImp(const LineData &d);

    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &si) const override;
    Rect surroundingRect() const override;

    ObjectImp *transform(const Transformation &) const override;

    const Coordinate getPoint(double param, const KigDocument &) const override;
    double getParam(const Coordinate &, const KigDocument &) const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &d) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    SegmentImp *copy() const override;

    /**
     * Get the length of this segment.
     */
    double length() const;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool containsPoint(const Coordinate &p, const KigDocument &doc) const override;
    bool internalContainsPoint(const Coordinate &p, double threshold) const;
};

/**
 * An ObjectImp representing a ray. This means half of a line, it is
 * infinite in one direction, but ends at a certain point in the other
 * direction.
 */
class RayImp : public AbstractLineImp
{
public:
    typedef AbstractLineImp Parent;
    /**
     * Returns the ObjectImpType representing the RayImp
     * type.
     */
    static const ObjectImpType *stype();

    /**
     * Construct a ray, starting at a, and going through b.
     */
    RayImp(const Coordinate &a, const Coordinate &b);
    /**
     * Construct a ray from a LineData.
     */
    explicit RayImp(const LineData &d);

    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &si) const override;
    Rect surroundingRect() const override;

    ObjectImp *transform(const Transformation &) const override;

    const Coordinate getPoint(double param, const KigDocument &) const override;
    double getParam(const Coordinate &, const KigDocument &) const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &d) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    RayImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool containsPoint(const Coordinate &p, const KigDocument &doc) const override;
    bool internalContainsPoint(const Coordinate &p, double threshold) const;
};

/**
 * An ObjectImp representing a line.
 */
class LineImp : public AbstractLineImp
{
public:
    typedef AbstractLineImp Parent;

    /**
     * Returns the ObjectImpType representing the LineImp
     * type.
     */
    static const ObjectImpType *stype();

    /**
     * Construct a LineImp going through points a and b.
     */
    LineImp(const Coordinate &a, const Coordinate &b);
    /**
     * Construct a LineImp from a LineData.
     */
    explicit LineImp(const LineData &d);
    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &si) const override;
    Rect surroundingRect() const override;

    ObjectImp *transform(const Transformation &) const override;

    const Coordinate getPoint(double param, const KigDocument &) const override;
    double getParam(const Coordinate &, const KigDocument &) const override;

    LineImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool containsPoint(const Coordinate &p, const KigDocument &doc) const override;
    bool internalContainsPoint(const Coordinate &p, double threshold) const;
};
