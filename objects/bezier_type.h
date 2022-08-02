// SPDX-FileCopyrightText: 2009 Petr Gajdos <pgajdos@suse.cz>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

/**
 * Bézier curve of degree 2
 */
class BezierQuadricType : public ArgsParserObjectType
{
    BezierQuadricType();
    ~BezierQuadricType();

public:
    static const BezierQuadricType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Bézier curve of degree 3
 */
class BezierCubicType : public ArgsParserObjectType
{
    BezierCubicType();
    ~BezierCubicType();

public:
    static const BezierCubicType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Bézier curve of degree n
 */
class BezierCurveType : public ObjectType
{
    BezierCurveType();
    ~BezierCurveType();

public:
    static const BezierCurveType *instance();

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
 * Rational Bézier curve of degree 2
 */
class RationalBezierQuadricType : public ArgsParserObjectType
{
    RationalBezierQuadricType();
    ~RationalBezierQuadricType();

public:
    static const RationalBezierQuadricType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Rational Bézier curve of degree 3
 */
class RationalBezierCubicType : public ArgsParserObjectType
{
    RationalBezierCubicType();
    ~RationalBezierCubicType();

public:
    static const RationalBezierCubicType *instance();

    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    bool canMove(const ObjectTypeCalcer &o) const override;
    bool isFreelyTranslatable(const ObjectTypeCalcer &o) const override;
    std::vector<ObjectCalcer *> movableParents(const ObjectTypeCalcer &ourobj) const override;
    void move(ObjectTypeCalcer &o, const Coordinate &to, const KigDocument &d) const override;
    const Coordinate moveReferencePoint(const ObjectTypeCalcer &o) const override;
};

/**
 * Rational Bézier curve of degree n
 */
class RationalBezierCurveType : public ObjectType
{
    RationalBezierCurveType();
    ~RationalBezierCurveType();

public:
    static const RationalBezierCurveType *instance();

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
