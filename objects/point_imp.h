// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_POINT_IMP_H
#define KIG_OBJECTS_POINT_IMP_H

#include "../misc/coordinate.h"
#include "object_imp.h"

/**
 * An ObjectImp representing a point.
 */
class PointImp : public ObjectImp
{
    Coordinate mc;

public:
    typedef ObjectImp Parent;
    /**
     * Returns the ObjectImpType representing PointImp's.
     */
    static const ObjectImpType *stype();

    /**
     * Construct a PointImp with coordinate c.
     */
    explicit PointImp(const Coordinate &c);
    ~PointImp();

    Rect surroundingRect() const override;
    Coordinate attachPoint() const override;

    /**
     * Get the coordinate of this PointImp.
     */
    const Coordinate &coordinate() const
    {
        return mc;
    }
    /**
     * Set the coordinate of this PointImp.
     */
    void setCoordinate(const Coordinate &c);

    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &) const override;
    bool inRect(const Rect &r, int width, const KigWidget &) const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &d) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    ObjectImp *transform(const Transformation &) const override;

    PointImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    void fillInNextEscape(QString &s, const KigDocument &) const override;
    bool canFillInNextEscape() const override;

    bool equals(const ObjectImp &rhs) const override;
};

class BogusPointImp : public PointImp
{
public:
    explicit BogusPointImp(const Coordinate &c);
    ~BogusPointImp();
    static const ObjectImpType *stype();
    const ObjectImpType *type() const override;
};

#endif
