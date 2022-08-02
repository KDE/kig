// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "object_constructor.h"

class TwoOrOneIntersectionConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype_std;
    const ArgsParserObjectType *mtype_special;
    ArgsParser margsparser;

public:
    TwoOrOneIntersectionConstructor(const ArgsParserObjectType *t_std,
                                    const ArgsParserObjectType *t_special,
                                    const char *iconfile,
                                    const struct ArgsParser::spec argsspecv[]);
    ~TwoOrOneIntersectionConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class ThreeTwoOneIntersectionConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype_std;
    const ArgsParserObjectType *mtype_special;
    ArgsParser margsparser;

public:
    ThreeTwoOneIntersectionConstructor(const ArgsParserObjectType *t_std,
                                       const ArgsParserObjectType *t_special,
                                       const char *iconfile,
                                       const struct ArgsParser::spec argsspecv[]);
    ~ThreeTwoOneIntersectionConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class PolygonVertexTypeConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype;
    ArgsParser margsparser;

public:
    PolygonVertexTypeConstructor();
    ~PolygonVertexTypeConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class PolygonSideTypeConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype;
    ArgsParser margsparser;

public:
    PolygonSideTypeConstructor();
    ~PolygonSideTypeConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

/*
 */

class PointSequenceConstructor : public ObjectConstructor
{
    const QString mdescname;
    const QString mdesc;
    const QString miconfile;
    const ObjectType *mtype;

public:
    PointSequenceConstructor(const QString &descname, const QString &desc, const QString &iconfile, const ObjectType *type);

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;
    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;
    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    // TODO: move here actual drawprelm when AbstractPolygon is in place!
    virtual void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const = 0;

    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

/*
 * classes PolygonBNPTypeConstructor, OpenPolygonTypeConstructor and
 *         BezierCurveTypeConstructor
 * all inherit from the PointSequenceConstructor
 */

class PolygonBNPTypeConstructor : public PointSequenceConstructor
{
public:
    PolygonBNPTypeConstructor();
    ~PolygonBNPTypeConstructor();

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
};

class OpenPolygonTypeConstructor : public PointSequenceConstructor
{
public:
    OpenPolygonTypeConstructor();
    ~OpenPolygonTypeConstructor();

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
};

class PolygonBCVConstructor : public ObjectConstructor
{
    const ObjectType *mtype;

public:
    PolygonBCVConstructor();
    ~PolygonBCVConstructor();

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;
    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;
    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
    int computeNsides(const Coordinate &c, const Coordinate &v, const Coordinate &cntrl, int &winding) const;
    Coordinate getRotatedCoord(const Coordinate &c1, const Coordinate &c2, double alpha) const;
};

class BezierCurveTypeConstructor : public PointSequenceConstructor
{
public:
    BezierCurveTypeConstructor();
    ~BezierCurveTypeConstructor();

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
};

class RationalBezierCurveTypeConstructor : public ObjectConstructor
{
public:
    RationalBezierCurveTypeConstructor();
    ~RationalBezierCurveTypeConstructor();

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;

    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const;
    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &d) const;

    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class MeasureTransportConstructor : public ObjectConstructor
{
    const ObjectType *mtype;

public:
    MeasureTransportConstructor();
    ~MeasureTransportConstructor();

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;
    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;
    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class ConicRadicalConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype;
    const ArgsParser mparser;

public:
    ConicRadicalConstructor();
    ~ConicRadicalConstructor();
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;

    bool isTransform() const override;
};

class LocusConstructor : public StandardConstructorBase
{
    ArgsParser margsparser;

public:
    LocusConstructor();
    ~LocusConstructor();
    /**
     * we override the wantArgs() function, since we need to see
     * something about the objects that an ArgsParser can't know about,
     * namely, whether the first point is a constrained point...
     */
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;

    bool isTransform() const override;
};

class GenericAffinityConstructor : public MergeObjectConstructor
{
public:
    GenericAffinityConstructor();
    ~GenericAffinityConstructor();
    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
};

class GenericProjectivityConstructor : public MergeObjectConstructor
{
public:
    GenericProjectivityConstructor();
    ~GenericProjectivityConstructor();
    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
};

class InversionConstructor : public MergeObjectConstructor
{
public:
    InversionConstructor();
    ~InversionConstructor();
};

class GenericIntersectionConstructor : public MergeObjectConstructor
{
public:
    GenericIntersectionConstructor();
    ~GenericIntersectionConstructor();

    bool isIntersection() const override;

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;
};

class MidPointOfTwoPointsConstructor : public StandardConstructorBase
{
    ArgsParser mparser;

public:
    MidPointOfTwoPointsConstructor();
    ~MidPointOfTwoPointsConstructor();
    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class GoldenPointOfTwoPointsConstructor : public StandardConstructorBase
{
    ArgsParser mparser;

public:
    GoldenPointOfTwoPointsConstructor();
    ~GoldenPointOfTwoPointsConstructor();
    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
};

class TestConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype;

public:
    TestConstructor(const ArgsParserObjectType *type, const QString &descname, const QString &desc, const QString &iconfile);
    ~TestConstructor();
    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;
    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
    bool isTransform() const override;
    bool isTest() const override;

    BaseConstructMode *constructMode(KigPart &doc) override;
};

class TangentConstructor : public MergeObjectConstructor
{
public:
    TangentConstructor();
    ~TangentConstructor();

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    //  QString selectStatement(
    //    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    //    const KigWidget& w ) const;
};

class CocConstructor : public MergeObjectConstructor
{
public:
    CocConstructor();
    ~CocConstructor();

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;
    //  QString selectStatement(
    //    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    //    const KigWidget& w ) const;
};

bool relativePrimes(int n, int p);

std::vector<ObjectCalcer *> removeDuplicatedPoints(std::vector<ObjectCalcer *> points);
bool coincidentPoints(const ObjectImp *p1, const ObjectImp *p2);
