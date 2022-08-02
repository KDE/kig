// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_TEXT_IMP_H
#define KIG_OBJECTS_TEXT_IMP_H

#include "object_imp.h"

#include "../misc/coordinate.h"
#include "../misc/rect.h"

class TextImp : public ObjectImp
{
    QString mtext;
    Coordinate mloc;
    bool mframe;
    // with this var, we keep track of the place we drew in, for use in
    // the contains() function..
    mutable Rect mboundrect;

public:
    typedef ObjectImp Parent;
    static const ObjectImpType *stype();

    Coordinate attachPoint() const override;
    TextImp(const QString &text, const Coordinate &loc, bool frame = false);
    TextImp *copy() const override;
    ~TextImp();

    ObjectImp *transform(const Transformation &) const override;

    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &) const override;
    bool inRect(const Rect &r, int width, const KigWidget &) const override;
    bool valid() const;
    Rect surroundingRect() const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &w) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    QString text() const;
    const Coordinate coordinate() const;
    bool hasFrame() const;

    bool equals(const ObjectImp &rhs) const override;
};

class NumericTextImp : public TextImp
{
    double mvalue;

public:
    typedef TextImp Parent;
    static const ObjectImpType *stype();
    NumericTextImp(const QString &text, const Coordinate &loc, bool frame, double value);
    NumericTextImp *copy() const override;
    double getValue() const;
    const ObjectImpType *type() const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &w) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;
};

class BoolTextImp : public TextImp
{
    bool mvalue;

public:
    typedef TextImp Parent;
    static const ObjectImpType *stype();
    BoolTextImp(const QString &text, const Coordinate &loc, bool frame, bool value);
    BoolTextImp *copy() const override;
    bool getValue() const;
    const ObjectImpType *type() const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &w) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;
};

#endif
