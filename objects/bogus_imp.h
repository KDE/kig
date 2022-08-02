// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BOGUS_IMP_H
#define BOGUS_IMP_H

#include "../misc/kigtransform.h"
#include "../misc/object_hierarchy.h"
#include "object_imp.h"

#include <QString>

/**
 * This is the base class for the so-called BogusImp's.  These
 * ObjectImp's are not really ObjectImp's, in that they don't
 * represent objects.  They exist because ObjectImp's also serve
 * another purpose, namely containing data.  They can all be loaded
 * and saved, and the only difference between these objects and normal
 * objects are that these serve *only* to be loaded and saved.  This
 * approach adds a lot of flexibility to the Kig system, and has
 * certainly proven itself very valuable.
 */
class BogusImp : public ObjectImp
{
    typedef ObjectImp Parent;

public:
    /**
     * Returns the ObjectImpType representing the BogusImp type.
     */
    static const ObjectImpType *stype();

    Coordinate attachPoint() const override;
    void draw(KigPainter &p) const override;
    bool contains(const Coordinate &p, int width, const KigWidget &w) const override;
    bool inRect(const Rect &r, int width, const KigWidget &w) const override;
    Rect surroundingRect() const override;

    ObjectImp *transform(const Transformation &) const override;
};

/**
 * This ObjectImp represents an invalid object.  If a calculation
 * fails, then often an InvalidImp is returned, indicating that the
 * generated object is invalid.
 */
class InvalidImp : public BogusImp
{
public:
    /**
     * Returns the ObjectImpType representing the InvalidImp type.
     */
    static const ObjectImpType *stype();
    typedef BogusImp Parent;

    /**
     * Construct a new InvalidImp.
     */
    InvalidImp();
    InvalidImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool canFillInNextEscape() const override;
    void fillInNextEscape(QString &s, const KigDocument &) const override;

    bool equals(const ObjectImp &rhs) const override;
};

/**
 * This ObjectImp is a BogusImp containing only a double value.
 */
class DoubleImp : public BogusImp
{
    double mdata;

public:
    /**
     * Returns the ObjectImpType representing the DoubleImp type.
     */
    static const ObjectImpType *stype();
    typedef BogusImp Parent;

    /**
     * Construct a new DoubleImp containing the value d.
     */
    explicit DoubleImp(const double d);

    /**
     * Get hold of the contained data.
     */
    double data() const
    {
        return mdata;
    }
    /**
     * Set the contained data to d.
     */
    void setData(double d)
    {
        mdata = d;
    }

    DoubleImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool canFillInNextEscape() const override;
    void fillInNextEscape(QString &s, const KigDocument &) const override;

    bool equals(const ObjectImp &rhs) const override;
};

/**
 * This ObjectImp is a BogusImp containing only an int value.
 */
class IntImp : public BogusImp
{
    int mdata;

public:
    /**
     * Returns the ObjectImpType representing the IntImp type.
     */
    static const ObjectImpType *stype();
    typedef BogusImp Parent;

    /**
     * Construct a new IntImp containing the value d.
     */
    explicit IntImp(const int d);

    /**
     * Get hold of the contained data.
     */
    int data() const
    {
        return mdata;
    }
    /**
     * Set the contained data to d.
     */
    void setData(int d)
    {
        mdata = d;
    }

    IntImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool canFillInNextEscape() const override;
    void fillInNextEscape(QString &s, const KigDocument &) const override;

    bool equals(const ObjectImp &rhs) const override;
};

/**
 * This ObjectImp is a BogusImp containing only a string value.
 */
class StringImp : public BogusImp
{
    QString mdata;

public:
    /**
     * Returns the ObjectImpType representing the StringImp type.
     */
    static const ObjectImpType *stype();
    typedef BogusImp Parent;

    /**
     * Construct a new StringImp containing the string d.
     */
    explicit StringImp(const QString &d);

    /**
     * Get hold of the contained data.
     */
    const QString &data() const
    {
        return mdata;
    }
    /**
     * Set the contained data.
     */
    void setData(const QString &s)
    {
        mdata = s;
    }

    StringImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool canFillInNextEscape() const override;
    void fillInNextEscape(QString &s, const KigDocument &) const override;

    bool equals(const ObjectImp &rhs) const override;
};

class HierarchyImp : public BogusImp
{
    ObjectHierarchy mdata;

public:
    static const ObjectImpType *stype();
    typedef BogusImp Parent;

    explicit HierarchyImp(const ObjectHierarchy &h);

    const ObjectHierarchy &data() const
    {
        return mdata;
    }

    HierarchyImp *copy() const override;
    const char *baseName() const;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool equals(const ObjectImp &rhs) const override;
};

/**
 * \internal Don't mistake this imp for something that draws a
 * transformed object.  It does something completely different.  It's
 * a pure data Imp, like DoubleImp and friends that serves only to
 * store the data of a transformation ( see the Transformation class
 * in ../misc/kigtransform.h
 */
class TransformationImp : public BogusImp
{
    Transformation mdata;

public:
    static const ObjectImpType *stype();
    typedef BogusImp Parent;

    explicit TransformationImp(const Transformation &h);

    const Transformation &data() const
    {
        return mdata;
    }
    void setData(const Transformation &h)
    {
        mdata = h;
    }

    TransformationImp *copy() const override;

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    bool equals(const ObjectImp &rhs) const override;
};

class TestResultImp : public StringImp
{
    bool mtruth;

public:
    static const ObjectImpType *stype();
    typedef StringImp Parent;

    TestResultImp(bool truth, const QString &s);

    TestResultImp *copy() const override;

    // const QString& data() const { return mdata; };
    bool truth() const
    {
        return mtruth;
    }

    const ObjectImpType *type() const override;
    void visit(ObjectImpVisitor *vtor) const override;

    int numberOfProperties() const override;
    const QList<KLazyLocalizedString> properties() const override;
    const QByteArrayList propertiesInternalNames() const override;
    ObjectImp *property(int which, const KigDocument &d) const override;
    const char *iconForProperty(int which) const override;
    const ObjectImpType *impRequirementForProperty(int which) const override;
    bool isPropertyDefinedOnOrThroughThisImp(int which) const override;

    bool equals(const ObjectImp &rhs) const override;
};

#endif
