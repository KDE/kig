// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_mode.h"

#include "../objects/object_calcer.h"

class ArgsParserObjectType;
class ObjectConstructor;
class ObjectCalcer;

class PointConstructMode : public BaseMode
{
    /**
     * this is the point that we move around, for the user to add
     * somewhere..
     */
    ObjectTypeCalcer::shared_ptr mpt;

public:
    explicit PointConstructMode(KigPart &d);
    ~PointConstructMode();

    using BaseMode::midClicked;
    using BaseMode::mouseMoved;
    using BaseMode::rightClicked;

protected:
    void leftClickedObject(ObjectHolder *o, const QPoint &p, KigWidget &w, bool ctrlOrShiftDown) override;
    void midClicked(const QPoint &p, KigWidget &w) override;
    void rightClicked(const std::vector<ObjectHolder *> &oco, const QPoint &p, KigWidget &w) override;
    void mouseMoved(const std::vector<ObjectHolder *> &os, const QPoint &p, KigWidget &w, bool shiftpressed) override;

    void enableActions() override;
    void cancelConstruction() override;

    void redrawScreen(KigWidget *) override;
};

class BaseConstructMode : public BaseMode
{
    /**
     * this is the point that we move around, in case the user wants to
     * add a point somewhere..
     */
    ObjectTypeCalcer::shared_ptr mpt;
    /**
     * mp: this point always follows the cursor
     *
     * IMPORTANT: this Calcer must NEVER be added to the document, since
     * it is used in constructors that need more input from the user
     * to decide parameters of the constructed object that will be fixed
     * afterwards (like the number of sides of a regular polygon)
     */
    ObjectTypeCalcer *mcursor;
    // we also allocate here the corresponding objectholder, since the
    // only sensible place where to deallocate it is in the destructor
    // of this class
    //  ObjectHolder* mcursorholder;
    std::vector<ObjectHolder *> mparents;

    void leftReleased(QMouseEvent *e, KigWidget *v) override;

public:
    using BaseMode::midClicked;
    using BaseMode::mouseMoved;
    using BaseMode::rightClicked;
    void selectObject(ObjectHolder *o, KigWidget &w);
    void selectObjects(const std::vector<ObjectHolder *> &os, KigWidget &w);
    virtual ~BaseConstructMode();

protected:
    BaseConstructMode(KigPart &d);

protected:
    void leftClickedObject(ObjectHolder *o, const QPoint &p, KigWidget &w, bool ctrlOrShiftDown) override;
    void midClicked(const QPoint &p, KigWidget &w) override;
    void rightClicked(const std::vector<ObjectHolder *> &oco, const QPoint &p, KigWidget &w) override;
    void mouseMoved(const std::vector<ObjectHolder *> &os, const QPoint &p, KigWidget &w, bool shiftpressed) override;

    void enableActions() override;
    void cancelConstruction() override;
    void finish();

protected:
    virtual void handlePrelim(const std::vector<ObjectCalcer *> &os, const QPoint &p, KigPainter &, KigWidget &w) = 0;
    virtual QString selectStatement(const std::vector<ObjectCalcer *> &args, const KigWidget &w) = 0;
    virtual int isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const int &) = 0;
    virtual int wantArgs(const std::vector<ObjectCalcer *> &, KigDocument &d, KigWidget &w) = 0;
    virtual void handleArgs(const std::vector<ObjectCalcer *> &args, KigWidget &w) = 0;

    void redrawScreen(KigWidget *) override;
};

class ConstructMode : public BaseConstructMode
{
    const ObjectConstructor *mctor;

public:
    ConstructMode(KigPart &d, const ObjectConstructor *ctor);
    ~ConstructMode();

    void handlePrelim(const std::vector<ObjectCalcer *> &os, const QPoint &p, KigPainter &, KigWidget &w) override;
    QString selectStatement(const std::vector<ObjectCalcer *> &args, const KigWidget &w) override;
    int isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const int &) override;
    int wantArgs(const std::vector<ObjectCalcer *> &, KigDocument &d, KigWidget &w) override;
    void handleArgs(const std::vector<ObjectCalcer *> &args, KigWidget &w) override;
};

/**
 * This class constructs a test object.  It has special needs over
 * ConstructMode because first the arguments need to be chosen, and
 * then the location for the resulting TextImp needs to be chosen.  It
 * also needs special code for the drawPrelim and wantArgs code.
 *
 * Therefore, we inherit from BaseConstructMode, and override the
 * event callbacks, so that this mode behaves like a
 * BaseConstructMode, until handleArgs is called.  After that, mresult
 * is no longer 0, and then the mode behaves in its own way, allowing
 * the user to choose a location for the new label object.
 */
class TestConstructMode : public BaseConstructMode
{
    const ArgsParserObjectType *mtype;
    ObjectCalcer::shared_ptr mresult;

public:
    TestConstructMode(KigPart &d, const ArgsParserObjectType *type);
    ~TestConstructMode();

    using BaseMode::midClicked;
    using BaseMode::mouseMoved;
    using BaseMode::rightClicked;

    void handlePrelim(const std::vector<ObjectCalcer *> &os, const QPoint &p, KigPainter &, KigWidget &w) override;
    QString selectStatement(const std::vector<ObjectCalcer *> &args, const KigWidget &w) override;
    int isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const int &) override;
    int wantArgs(const std::vector<ObjectCalcer *> &, KigDocument &d, KigWidget &w) override;
    void handleArgs(const std::vector<ObjectCalcer *> &args, KigWidget &w) override;

    void leftClickedObject(ObjectHolder *o, const QPoint &p, KigWidget &w, bool ctrlOrShiftDown) override;
    void midClicked(const QPoint &p, KigWidget &w) override;
    void rightClicked(const std::vector<ObjectHolder *> &oco, const QPoint &p, KigWidget &w) override;
    void mouseMoved(const std::vector<ObjectHolder *> &os, const QPoint &p, KigWidget &w, bool shiftpressed) override;
};
