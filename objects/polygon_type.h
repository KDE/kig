// SPDX-FileCopyrightText: 2003 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

/**
 * Triangle by its vertices
 */
class TriangleB3PType : public ArgsParserObjectType
{
    TriangleB3PType();
    ~TriangleB3PType();

public:
    static const TriangleB3PType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Polygon by its vertices
 */
class PolygonBNPType : public ObjectType
{
    PolygonBNPType();
    ~PolygonBNPType();

public:
    static const PolygonBNPType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;
    bool isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const override;
    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &args) const override;
    Args sortArgs(const Args &args) const override;

    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Open Polygon (Polyline, Polygonal Line)
 */
class OpenPolygonType : public ObjectType
{
    OpenPolygonType();
    ~OpenPolygonType();

public:
    static const OpenPolygonType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;
    bool isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const override;
    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &args) const override;
    Args sortArgs(const Args &args) const override;

    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Polygon by center and vertex
 */
class PolygonBCVType : public ObjectType
{
    PolygonBCVType();
    ~PolygonBCVType();

public:
    static const PolygonBCVType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;

    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;
    bool isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const override;
    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &args) const override;
    Args sortArgs(const Args &args) const override;
    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

int polygonlineintersection(const std::vector<Coordinate> &ppoints,
                            const Coordinate &a,
                            const Coordinate &b,
                            double &t1,
                            double &t2,
                            std::vector<Coordinate>::const_iterator &intersectionside);

int polygonlineintersection(const std::vector<Coordinate> &ppoints,
                            const Coordinate &a,
                            const Coordinate &b,
                            bool boundleft,
                            bool boundright,
                            bool inside,
                            bool openpolygon,
                            double &t1,
                            double &t2,
                            std::vector<Coordinate>::const_iterator &intersectionside);

class PolygonLineIntersectionType : public ArgsParserObjectType
{
    PolygonLineIntersectionType();

protected:
    PolygonLineIntersectionType(const char fulltypename[], const struct ArgsParser::spec argsspec[], int n);
    ~PolygonLineIntersectionType();

public:
    static const PolygonLineIntersectionType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class OPolygonalLineIntersectionType : public PolygonLineIntersectionType
{
    OPolygonalLineIntersectionType();
    ~OPolygonalLineIntersectionType();

public:
    static const OPolygonalLineIntersectionType *instance();
    const ObjectImpType *resultId() const override;
};

class CPolygonalLineIntersectionType : public PolygonLineIntersectionType
{
    CPolygonalLineIntersectionType();
    ~CPolygonalLineIntersectionType();

public:
    static const CPolygonalLineIntersectionType *instance();
    const ObjectImpType *resultId() const override;
};

class PolygonPolygonIntersectionType : public ArgsParserObjectType
{
    PolygonPolygonIntersectionType();
    ~PolygonPolygonIntersectionType();

public:
    static const PolygonPolygonIntersectionType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class PolygonVertexType : public ArgsParserObjectType
{
    PolygonVertexType();
    ~PolygonVertexType();

public:
    static const PolygonVertexType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class PolygonSideType : public ArgsParserObjectType
{
    PolygonSideType();
    ~PolygonSideType();

public:
    static const PolygonSideType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConvexHullType : public ArgsParserObjectType
{
    ConvexHullType();
    ~ConvexHullType();

public:
    static const ConvexHullType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
