// SPDX-FileCopyrightText: 2002-2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "argsparser.h"
#include "object_hierarchy.h"

class KigPainter;
class KigDocument;
class KigGUIAction;
class KigWidget;
class ArgsParserObjectType;
class ObjectType;
class BaseConstructMode;

class QString;
class QByteArray;

/**
 * This class represents a way to construct a set of objects from a
 * set of other objects.  There are some important child classes, like
 * MacroConstructor, StandardObjectConstructor etc. ( see below )
 * Actually, it is more generic than that, it provides a way to do
 * _something_ with a set of objects, but for now, i only use it to
 * construct objects.  Maybe some day, i'll find something more
 * interesting to do with it, who knows... ;)
 */
class ObjectConstructor
{
public:
    virtual ~ObjectConstructor();

    virtual const QString descriptiveName() const = 0;
    virtual const QString description() const = 0;
    virtual const QString iconFileName(const bool canBeNull = false) const = 0;

    /**
     * the following function is called in case of duplication of arguments
     * and returns true if this is acceptable; this will return false for
     * typical objects
     */
    virtual bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const = 0;
    /**
     * can this constructor do something useful with \p os ?  return
     * ArgsParser::Complete, Valid or NotGood
     */
    virtual int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const = 0;

    /**
     * do something fun with \p os .. This func is only called if wantArgs
     * returned Complete. handleArgs should <i>not</i> do any
     * drawing. after somebody calls this function, he should
     * redrawScreen() himself.
     */
    virtual void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const = 0;

    /**
     * return a string describing what you would use \p o for if it were
     * selected...  \p o should be part of \p sel .
     */
    virtual QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const = 0;

    /**
     * return a string describing what argument you want next, if the
     * given selection of objects were selected.
     */
    virtual QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const = 0;

    /**
     * show a preliminary version of what you would do when \ref handleArgs
     * would be called. E.g. if this constructor normally constructs a
     * locus through some 5 points, then it will try to draw a locus
     * through whatever number of points it gets.
     */
    virtual void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const = 0;

    virtual void plug(KigPart *doc, KigGUIAction *kact) = 0;

    virtual bool isTransform() const = 0;
    virtual bool isTest() const;
    virtual bool isIntersection() const;

    /**
     * Which construct mode should be used for this ObjectConstructor.
     * In fact, this is not a pretty design.  The Kig
     * GUIAction-ObjectConstructor stuff should be reworked into a
     * general GUIAction, which just models something which can be
     * executed given a certain number of arguments.  The code for
     * drawPrelim and such should all be in the ConstructMode, and the
     * new GUIAction should just start the correct KigMode with the
     * correct arguments.
     *
     * This function is only overridden in TestConstructor.
     */
    virtual BaseConstructMode *constructMode(KigPart &doc);
};

/**
 * This class provides wraps ObjectConstructor in a more simple
 * interface for the most common object types.
 */
class StandardConstructorBase : public ObjectConstructor
{
    QString mdescname;
    QString mdesc;
    QString miconfile;
    const ArgsParser &margsparser;

public:
    StandardConstructorBase(const QString &descname, const QString &desc, const QString &iconfile, const ArgsParser &parser);

    virtual ~StandardConstructorBase();

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;

    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;

    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    virtual void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const = 0;

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    virtual std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const = 0;
};

/**
 * A standard implementation of StandardConstructorBase for simple
 * types.
 */
class SimpleObjectTypeConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype;

public:
    explicit SimpleObjectTypeConstructor(const ArgsParserObjectType *t, const QString &descname, const QString &desc, const QString &iconfile);

    ~SimpleObjectTypeConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;

    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;

    void plug(KigPart *doc, KigGUIAction *kact) override;

    bool isTransform() const override;
};

/**
 * A standard implementation of StandardConstructorBase for property
 * objects...
 */
class PropertyObjectConstructor : public StandardConstructorBase
{
    ArgsParser mparser;
    QByteArray mpropinternalname;

public:
    explicit PropertyObjectConstructor(const ObjectImpType *imprequirement,
                                       const QString &usetext,
                                       const QString &selectstat,
                                       const QString &descname,
                                       const QString &desc,
                                       const QString &iconfile,
                                       const QByteArray &propertyinternalname);

    ~PropertyObjectConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;

    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;

    void plug(KigPart *doc, KigGUIAction *kact) override;

    bool isTransform() const override;
};

/**
 * This class is the equivalent of \ref SimpleObjectTypeConstructor
 * for object types that are constructed in groups of more than one.
 * For example, the intersection of a circle and line in general
 * produces two points, in general.  Internally, we differentiate
 * between them by passing them a parameter of ( in this case ) 1 or
 * -1.  There are still other object types that work the same, and
 * they all require this sort of parameter.
 * E.g. CubicLineIntersectionType takes a parameter between 1 and 3.
 * This class knows about that, and constructs the objects along this
 * scheme..
 */
class MultiObjectTypeConstructor : public StandardConstructorBase
{
    const ArgsParserObjectType *mtype;
    std::vector<int> mparams;
    ArgsParser mparser;

public:
    explicit MultiObjectTypeConstructor(const ArgsParserObjectType *t,
                                        const QString &descname,
                                        const QString &desc,
                                        const QString &iconfile,
                                        const std::vector<int> &params);
    explicit MultiObjectTypeConstructor(const ArgsParserObjectType *t,
                                        const QString &descname,
                                        const QString &desc,
                                        const QString &iconfile,
                                        int a,
                                        int b,
                                        int c = -999,
                                        int d = -999);
    ~MultiObjectTypeConstructor();

    void drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &) const override;

    std::vector<ObjectHolder *> build(const std::vector<ObjectCalcer *> &os, KigDocument &d, KigWidget &w) const override;

    void plug(KigPart *doc, KigGUIAction *kact) override;

    bool isTransform() const override;
};

/**
 * This class is a collection of some other ObjectConstructors, that
 * makes them appear to the user as a single ObjectConstructor.  It is
 * e.g. used for the "intersection" constructor.
 */
class MergeObjectConstructor : public ObjectConstructor
{
    const QString mdescname;
    const QString mdesc;
    const QString miconfilename;
    typedef std::vector<ObjectConstructor *> vectype;
    vectype mctors;

public:
    MergeObjectConstructor(const QString &descname, const QString &desc, const QString &iconfilename);
    ~MergeObjectConstructor();

    void merge(ObjectConstructor *e);

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;

    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;

    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    void plug(KigPart *doc, KigGUIAction *kact) override;

    bool isTransform() const override;
};

/**
 * MacroConstructor is a class that represents Kig macro's: these are
 * constructed by the user, and defined by a set of input and a set of
 * output objects.  The macro-constructor saves the way in which the
 * output objects have been built from the input objects, and when
 * given similar input objects, it will produce objects in the given
 * way.  The data is saved in a \ref ObjectHierarchy.
 */
class MacroConstructor : public ObjectConstructor
{
    ObjectHierarchy mhier;
    QString mname;
    QString mdesc;
    bool mbuiltin;
    QByteArray miconfile;
    ArgsParser mparser;

public:
    MacroConstructor(const std::vector<ObjectCalcer *> &input,
                     const std::vector<ObjectCalcer *> &output,
                     const QString &name,
                     const QString &description,
                     const QByteArray &iconfile = nullptr);
    MacroConstructor(const ObjectHierarchy &hier, const QString &name, const QString &desc, const QByteArray &iconfile = nullptr);
    ~MacroConstructor();

    const ObjectHierarchy &hierarchy() const;

    const QString descriptiveName() const override;
    const QString description() const override;
    const QString iconFileName(const bool canBeNull = false) const override;

    bool isAlreadySelectedOK(const std::vector<ObjectCalcer *> &os, const uint &) const override;
    int wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const override;

    void handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const override;

    QString useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    QString selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const override;

    void handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const override;

    void plug(KigPart *doc, KigGUIAction *kact) override;

    void setBuiltin(bool builtin);

    /**
     * is this the ctor for a transformation type.  We want to know this
     * cause transform types are shown separately in an object's RMB
     * menu..
     */
    bool isTransform() const override;

    void setName(const QString &name);
    void setDescription(const QString &desc);
    void setIcon(QByteArray &icon);
};
