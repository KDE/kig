// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "object_constructor.h"

#include "argsparser.h"
#include "guiaction.h"
#include "kigpainter.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include "../objects/bogus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/line_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/object_holder.h"
#include "../objects/object_imp.h"
#include "../objects/object_type.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"

#include "../modes/construct_mode.h"

#include <QPen>

#include <algorithm>
#include <functional>
#include <iterator>

const QString StandardConstructorBase::descriptiveName() const
{
    return mdescname;
}

const QString StandardConstructorBase::description() const
{
    return mdesc;
}

const QString StandardConstructorBase::iconFileName(const bool) const
{
    return miconfile;
}

bool StandardConstructorBase::isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const uint &) const
{
    return false;
}

StandardConstructorBase::StandardConstructorBase(const QString &descname, const QString &desc, const QString &iconfile, const ArgsParser &parser)
    : mdescname(descname)
    , mdesc(desc)
    , miconfile(iconfile)
    , margsparser(parser)
{
}

int StandardConstructorBase::wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &, const KigWidget &) const
{
    return margsparser.check(os);
}

void StandardConstructorBase::handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const
{
    std::vector<ObjectHolder *> bos = build(os, d.document(), v);
    for (std::vector<ObjectHolder *>::iterator i = bos.begin(); i != bos.end(); ++i) {
        (*i)->calc(d.document());
    }

    d.addObjects(bos);
}

void StandardConstructorBase::handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &) const
{
    assert(margsparser.check(os) != ArgsParser::Invalid);
    std::vector<ObjectCalcer *> args = margsparser.parse(os);
    p.setBrushStyle(Qt::NoBrush);
    p.setBrushColor(Qt::red);
    p.setPen(QPen(Qt::red, 1));
    p.setWidth(-1); // -1 means the default width for the object being
                    // drawn..

    ObjectDrawer drawer(Qt::red);
    drawprelim(drawer, p, args, d);
}

SimpleObjectTypeConstructor::SimpleObjectTypeConstructor(const ArgsParserObjectType *t, const QString &descname, const QString &desc, const QString &iconfile)
    : StandardConstructorBase(descname, desc, iconfile, t->argsParser())
    , mtype(t)
{
}

SimpleObjectTypeConstructor::~SimpleObjectTypeConstructor()
{
}

void SimpleObjectTypeConstructor::drawprelim(const ObjectDrawer &drawer,
                                             KigPainter &p,
                                             const std::vector<ObjectCalcer *> &parents,
                                             const KigDocument &doc) const
{
    Args args;
    using namespace std;
    transform(parents.begin(), parents.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));
    ObjectImp *data = mtype->calc(args, doc);
    drawer.draw(*data, p, true);
    delete data;
}

std::vector<ObjectHolder *> SimpleObjectTypeConstructor::build(const std::vector<ObjectCalcer *> &os, KigDocument &, KigWidget &) const
{
    ObjectTypeCalcer *calcer = new ObjectTypeCalcer(mtype, os);
    ObjectHolder *h = new ObjectHolder(calcer);
    std::vector<ObjectHolder *> ret;
    ret.push_back(h);
    return ret;
}

StandardConstructorBase::~StandardConstructorBase()
{
}

MultiObjectTypeConstructor::MultiObjectTypeConstructor(const ArgsParserObjectType *t,
                                                       const QString &descname,
                                                       const QString &desc,
                                                       const QString &iconfile,
                                                       const std::vector<int> &params)
    : StandardConstructorBase(descname, desc, iconfile, mparser)
    , mtype(t)
    , mparams(params)
    , mparser(t->argsParser().without(IntImp::stype()))
{
}

MultiObjectTypeConstructor::MultiObjectTypeConstructor(const ArgsParserObjectType *t,
                                                       const QString &descname,
                                                       const QString &desc,
                                                       const QString &iconfile,
                                                       int a,
                                                       int b,
                                                       int c,
                                                       int d)
    : StandardConstructorBase(descname, desc, iconfile, mparser)
    , mtype(t)
    , mparams()
    , mparser(t->argsParser().without(IntImp::stype()))
{
    mparams.push_back(a);
    mparams.push_back(b);
    if (c != -999)
        mparams.push_back(c);
    if (d != -999)
        mparams.push_back(d);
}

MultiObjectTypeConstructor::~MultiObjectTypeConstructor()
{
}

void MultiObjectTypeConstructor::drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &doc) const
{
    Args args;
    using namespace std;
    transform(parents.begin(), parents.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));

    for (vector<int>::const_iterator i = mparams.begin(); i != mparams.end(); ++i) {
        IntImp param(*i);
        args.push_back(&param);
        ObjectImp *data = mtype->calc(args, doc);
        drawer.draw(*data, p, true);
        delete data;
        args.pop_back();
    };
}

std::vector<ObjectHolder *> MultiObjectTypeConstructor::build(const std::vector<ObjectCalcer *> &os, KigDocument &, KigWidget &) const
{
    std::vector<ObjectHolder *> ret;
    for (std::vector<int>::const_iterator i = mparams.begin(); i != mparams.end(); ++i) {
        ObjectConstCalcer *d = new ObjectConstCalcer(new IntImp(*i));

        std::vector<ObjectCalcer *> args(os);
        args.push_back(d);

        ret.push_back(new ObjectHolder(new ObjectTypeCalcer(mtype, args)));
    };
    return ret;
}

MergeObjectConstructor::~MergeObjectConstructor()
{
    for (vectype::iterator i = mctors.begin(); i != mctors.end(); ++i)
        delete *i;
}

MergeObjectConstructor::MergeObjectConstructor(const QString &descname, const QString &desc, const QString &iconfilename)
    : ObjectConstructor()
    , mdescname(descname)
    , mdesc(desc)
    , miconfilename(iconfilename)
    , mctors()
{
}

ObjectConstructor::~ObjectConstructor()
{
}

void MergeObjectConstructor::merge(ObjectConstructor *e)
{
    mctors.push_back(e);
}

const QString MergeObjectConstructor::descriptiveName() const
{
    return mdescname;
}

const QString MergeObjectConstructor::description() const
{
    return mdesc;
}

const QString MergeObjectConstructor::iconFileName(const bool) const
{
    return miconfilename;
}

bool MergeObjectConstructor::isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const uint &) const
{
    return false;
}

int MergeObjectConstructor::wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &d, const KigWidget &v) const
{
    for (vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i) {
        int w = (*i)->wantArgs(os, d, v);
        if (w != ArgsParser::Invalid)
            return w;
    };
    return ArgsParser::Invalid;
}

void MergeObjectConstructor::handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &v) const
{
    for (vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i) {
        int w = (*i)->wantArgs(os, d.document(), v);
        if (w == ArgsParser::Complete) {
            (*i)->handleArgs(os, d, v);
            return;
        };
    };
    assert(false);
}

void MergeObjectConstructor::handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const
{
    for (vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i) {
        int w = (*i)->wantArgs(sel, d, v);
        if (w != ArgsParser::Invalid) {
            (*i)->handlePrelim(p, sel, d, v);
            return;
        };
    };
}

QString StandardConstructorBase::useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &, const KigWidget &) const
{
    using namespace std;
    Args args;
    transform(sel.begin(), sel.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));

    std::string ret = margsparser.usetext(o.imp(), args);
    if (ret.empty())
        return QString();
    return i18n(ret.c_str());
}

QString StandardConstructorBase::selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &, const KigWidget &) const
{
    using namespace std;
    Args args;
    transform(sel.begin(), sel.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));

    std::string ret = margsparser.selectStatement(args);
    if (ret.empty())
        return QString();
    return i18n(ret.c_str());
}

QString MergeObjectConstructor::useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &v) const
{
    for (vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i) {
        std::vector<ObjectCalcer *> args(sel);
        int w = (*i)->wantArgs(args, d, v);
        if (w != ArgsParser::Invalid)
            return (*i)->useText(o, sel, d, v);
    };
    return QString();
}

QString MergeObjectConstructor::selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &d, const KigWidget &w) const
{
    for (vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i) {
        std::vector<ObjectCalcer *> args(sel);
        int wa = (*i)->wantArgs(args, d, w);
        if (wa != ArgsParser::Invalid)
            return (*i)->selectStatement(sel, d, w);
    };
    return QString();
}

MacroConstructor::MacroConstructor(const ObjectHierarchy &hier, const QString &name, const QString &desc, const QByteArray &iconfile)
    : ObjectConstructor()
    , mhier(hier)
    , mname(name)
    , mdesc(desc)
    , mbuiltin(false)
    , miconfile(iconfile)
    , mparser(mhier.argParser())
{
}

MacroConstructor::MacroConstructor(const std::vector<ObjectCalcer *> &input,
                                   const std::vector<ObjectCalcer *> &output,
                                   const QString &name,
                                   const QString &description,
                                   const QByteArray &iconfile)
    : ObjectConstructor()
    , mhier(input, output)
    , mname(name)
    , mdesc(description)
    , mbuiltin(false)
    , miconfile(iconfile)
    , mparser(mhier.argParser())
{
}

MacroConstructor::~MacroConstructor()
{
}

const QString MacroConstructor::descriptiveName() const
{
    return mname;
}

const QString MacroConstructor::description() const
{
    return mdesc;
}

const QString MacroConstructor::iconFileName(const bool canBeNull) const
{
    return (miconfile.isNull() && !canBeNull) ? QStringLiteral("system-run") : miconfile;
}

bool MacroConstructor::isAlreadySelectedOK(const std::vector<ObjectCalcer *> &, const uint &) const
{
    return false;
}

int MacroConstructor::wantArgs(const std::vector<ObjectCalcer *> &os, const KigDocument &, const KigWidget &) const
{
    return mparser.check(os);
}

void MacroConstructor::handleArgs(const std::vector<ObjectCalcer *> &os, KigPart &d, KigWidget &) const
{
    std::vector<ObjectCalcer *> args = mparser.parse(os);
    std::vector<ObjectCalcer *> bos = mhier.buildObjects(args, d.document());
    std::vector<ObjectHolder *> hos;
    for (std::vector<ObjectCalcer *>::iterator i = bos.begin(); i != bos.end(); ++i) {
        hos.push_back(new ObjectHolder(*i));
        hos.back()->calc(d.document());
    }

    d.addObjects(hos);
}

QString MacroConstructor::selectStatement(const std::vector<ObjectCalcer *> &sel, const KigDocument &, const KigWidget &) const
{
    using namespace std;
    Args args;
    transform(sel.begin(), sel.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));
    std::string ret = mparser.selectStatement(args);
    if (ret.empty())
        return QString();
    else
        return i18n(ret.c_str());
}

QString MacroConstructor::useText(const ObjectCalcer &o, const std::vector<ObjectCalcer *> &sel, const KigDocument &, const KigWidget &) const
{
    using namespace std;
    Args args;
    transform(sel.begin(), sel.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));
    std::string ret = mparser.usetext(o.imp(), args);
    if (ret.empty())
        return QString();
    else
        return i18n(ret.c_str());
}

void MacroConstructor::handlePrelim(KigPainter &p, const std::vector<ObjectCalcer *> &sel, const KigDocument &doc, const KigWidget &) const
{
    if (sel.size() != mhier.numberOfArgs())
        return;

    using namespace std;
    Args args;
    transform(sel.begin(), sel.end(), back_inserter(args), std::mem_fn(&ObjectCalcer::imp));
    args = mparser.parse(args);
    std::vector<ObjectImp *> ret = mhier.calc(args, doc);
    for (uint i = 0; i < ret.size(); ++i) {
        ObjectDrawer d;
        d.draw(*ret[i], p, true);
        ret[i]->draw(p);
        delete ret[i];
    };
}

void SimpleObjectTypeConstructor::plug(KigPart *, KigGUIAction *)
{
}

void MultiObjectTypeConstructor::plug(KigPart *, KigGUIAction *)
{
}

void MergeObjectConstructor::plug(KigPart *, KigGUIAction *)
{
}

void MacroConstructor::plug(KigPart *doc, KigGUIAction *kact)
{
    if (mbuiltin)
        return;
    if (mhier.numberOfResults() != 1)
        doc->aMNewOther.append(kact);
    else {
        if (mhier.idOfLastResult() == SegmentImp::stype())
            doc->aMNewSegment.append(kact);
        else if (mhier.idOfLastResult() == PointImp::stype())
            doc->aMNewPoint.append(kact);
        else if (mhier.idOfLastResult() == CircleImp::stype())
            doc->aMNewCircle.append(kact);
        else if (mhier.idOfLastResult()->inherits(AbstractLineImp::stype()))
            // line or ray
            doc->aMNewLine.append(kact);
        else if (mhier.idOfLastResult() == ConicImp::stype())
            doc->aMNewConic.append(kact);
        else
            doc->aMNewOther.append(kact);
    };
    doc->aMNewAll.append(kact);
}

const ObjectHierarchy &MacroConstructor::hierarchy() const
{
    return mhier;
}

bool SimpleObjectTypeConstructor::isTransform() const
{
    return mtype->isTransform();
}

bool MultiObjectTypeConstructor::isTransform() const
{
    return mtype->isTransform();
}

bool MergeObjectConstructor::isTransform() const
{
    bool ret = false;
    for (vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i)
        ret |= (*i)->isTransform();
    return ret;
}

bool MacroConstructor::isTransform() const
{
    return false;
}

void MacroConstructor::setBuiltin(bool builtin)
{
    mbuiltin = builtin;
}

bool ObjectConstructor::isIntersection() const
{
    return false;
}

PropertyObjectConstructor::PropertyObjectConstructor(const ObjectImpType *imprequirement,
                                                     const QString &usetext,
                                                     const QString &selectstat,
                                                     const QString &descname,
                                                     const QString &desc,
                                                     const QString &iconfile,
                                                     const QByteArray &propertyinternalname)
    : StandardConstructorBase(descname, desc, iconfile, mparser)
    , mpropinternalname(propertyinternalname)
{
    ArgsParser::spec argsspec[1];
    argsspec[0].type = imprequirement;
    argsspec[0].usetext = usetext;
    argsspec[0].selectstat = selectstat;
    mparser.initialize(argsspec, 1);
}

PropertyObjectConstructor::~PropertyObjectConstructor()
{
}

void PropertyObjectConstructor::drawprelim(const ObjectDrawer &drawer, KigPainter &p, const std::vector<ObjectCalcer *> &parents, const KigDocument &d) const
{
    int index = parents[0]->imp()->propertiesInternalNames().indexOf(mpropinternalname);
    assert(index != -1);
    ObjectImp *imp = parents[0]->imp()->property(index, d);
    drawer.draw(*imp, p, true);
    delete imp;
}

std::vector<ObjectHolder *> PropertyObjectConstructor::build(const std::vector<ObjectCalcer *> &parents, KigDocument &, KigWidget &) const
{
    //  int index = parents[0]->imp()->propertiesInternalNames().indexOf( mpropinternalname );
    //  assert( index != -1 );
    std::vector<ObjectHolder *> ret;
    ret.push_back(new ObjectHolder(new ObjectPropertyCalcer(parents[0], mpropinternalname)));
    return ret;
}

void PropertyObjectConstructor::plug(KigPart *, KigGUIAction *)
{
}

bool PropertyObjectConstructor::isTransform() const
{
    return false;
}

bool ObjectConstructor::isTest() const
{
    return false;
}

BaseConstructMode *ObjectConstructor::constructMode(KigPart &doc)
{
    return new ConstructMode(doc, this);
}

void MacroConstructor::setName(const QString &name)
{
    mname = name;
}

void MacroConstructor::setDescription(const QString &desc)
{
    mdesc = desc;
}

void MacroConstructor::setIcon(QByteArray &icon)
{
    miconfile = icon;
}
