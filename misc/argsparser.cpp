// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "argsparser.h"

#include "../objects/object_holder.h"
#include "../objects/object_imp.h"

#include <QDebug>
#include <algorithm>
#include <cassert>

void ArgsParser::initialize(const struct spec *args, int n)
{
    std::vector<spec> vect(args, args + n);
    initialize(vect);
}

ArgsParser::ArgsParser()
{
}

ArgsParser::ArgsParser(const std::vector<spec> &args)
{
    initialize(args);
}

void ArgsParser::initialize(const std::vector<spec> &args)
{
    margs = args;
}

ArgsParser::ArgsParser(const struct spec *args, int n)
{
    initialize(args, n);
}

static bool hasimp(const ObjectCalcer &o, const ObjectImpType *imptype)
{
    return o.imp()->inherits(imptype);
}

static bool hasimp(const ObjectImp &o, const ObjectImpType *imptype)
{
    return o.inherits(imptype);
}

static bool isvalid(const ObjectImp &o)
{
    return o.valid();
}

static bool isvalid(const ObjectCalcer &o)
{
    return o.imp()->valid();
}

// we use a template method that is used for both Objects and Args to
// not have to write the same thing twice..
template<class Collection>
static int check(const Collection &c, const std::vector<ArgsParser::spec> &margs)
{
    std::vector<bool> found(margs.size());

    for (typename Collection::const_iterator o = c.begin(); o != c.end(); ++o) {
        for (uint i = 0; i < margs.size(); ++i) {
            if (hasimp(**o, margs[i].type) && !found[i]) {
                // object o is of a type that we're looking for
                found[i] = true;
                goto matched;
            };
        };
        return ArgsParser::Invalid;
    matched:;
    };
    for (uint i = 0; i < margs.size(); ++i)
        if (!found[i])
            return ArgsParser::Valid;
    return ArgsParser::Complete;
}

int ArgsParser::check(const Args &os) const
{
    return ::check(os, margs);
}

int ArgsParser::check(const std::vector<ObjectCalcer *> &os) const
{
    return ::check(os, margs);
}

template<typename Collection>
static Collection parse(const Collection &os, const std::vector<ArgsParser::spec> &margs)
{
    Collection ret(margs.size(), static_cast<typename Collection::value_type>(nullptr));

    for (typename Collection::const_iterator o = os.begin(); o != os.end(); ++o) {
        for (uint i = 0; i < margs.size(); ++i)
            if (hasimp(**o, margs[i].type) && ret[i] == nullptr) {
                // object o is of a type that we're looking for
                ret[i] = *o;
                goto added;
            }
    added:;
    };
    // remove 0's from the output..
    ret.erase(std::remove(ret.begin(), ret.end(), static_cast<typename Collection::value_type>(nullptr)), ret.end());
    return ret;
}

Args ArgsParser::parse(const Args &os) const
{
    return ::parse(os, margs);
}

std::vector<ObjectCalcer *> ArgsParser::parse(const std::vector<ObjectCalcer *> &os) const
{
    return ::parse(os, margs);
}

ArgsParser ArgsParser::without(const ObjectImpType *type) const
{
    std::vector<spec> ret;
    ret.reserve(margs.size() - 1);
    for (uint i = 0; i < margs.size(); ++i)
        if (margs[i].type != type)
            ret.push_back(margs[i]);
    return ArgsParser(ret);
}

ArgsParser::spec ArgsParser::findSpec(const ObjectImp *obj, const Args &parents) const
{
    spec ret;
    ret.type = nullptr;

    std::vector<bool> found(margs.size(), false);

    for (Args::const_iterator o = parents.begin(); o != parents.end(); ++o) {
        for (uint i = 0; i < margs.size(); ++i) {
            if ((*o)->inherits(margs[i].type) && !found[i]) {
                // object o is of a type that we're looking for
                found[i] = true;
                if (*o == obj)
                    return margs[i];
                // I know that "goto's are *evil*", but they're very useful
                // and completely harmless if you use them as better "break;"
                // statements.
                goto matched;
            };
        };
    matched:;
    }
    qDebug() << "no proper spec found :(";
    return ret;
}

const ObjectImpType *ArgsParser::impRequirement(const ObjectImp *o, const Args &parents) const
{
    spec s = findSpec(o, parents);
    return s.type;
}

std::string ArgsParser::usetext(const ObjectImp *obj, const Args &sel) const
{
    spec s = findSpec(obj, sel);
    if (std::holds_alternative<KLazyLocalizedString>(s.usetext)) {
        return std::get<KLazyLocalizedString>(s.usetext).toString().toStdString();
    } else {
        return std::get<QString>(s.usetext).toStdString();
    }
}

template<typename Collection>
static bool checkArgs(const Collection &os, uint min, const std::vector<ArgsParser::spec> &argsspec)
{
    assert(os.size() <= argsspec.size());
    if (os.size() < min)
        return false;
    uint checknum = os.size();
    for (uint i = 0; i < checknum; ++i) {
        if (!isvalid(*os[i]))
            return false;
        if (!hasimp(*os[i], argsspec[i].type))
            return false;
    }
    return true;
}

bool ArgsParser::checkArgs(const Args &os) const
{
    return checkArgs(os, margs.size());
}

bool ArgsParser::checkArgs(const Args &os, uint min) const
{
    return ::checkArgs(os, min, margs);
}

bool ArgsParser::checkArgs(const std::vector<ObjectCalcer *> &os) const
{
    return checkArgs(os, margs.size());
}

bool ArgsParser::checkArgs(const std::vector<ObjectCalcer *> &os, uint minobjects) const
{
    return ::checkArgs(os, minobjects, margs);
}

ArgsParser::~ArgsParser()
{
}

bool ArgsParser::isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const
{
    spec s = findSpec(o, parents);
    return s.onOrThrough;
}

std::string ArgsParser::selectStatement(const Args &selection) const
{
    std::vector<bool> found(margs.size(), false);

    for (Args::const_iterator o = selection.begin(); o != selection.end(); ++o) {
        for (uint i = 0; i < margs.size(); ++i) {
            if ((*o)->inherits(margs[i].type) && !found[i]) {
                // object o is of a type that we're looking for
                found[i] = true;
                break;
            }
        }
    }
    for (uint i = 0; i < margs.size(); ++i) {
        if (!found[i]) {
            if (std::holds_alternative<KLazyLocalizedString>(margs[i].selectstat)) {
                return std::get<KLazyLocalizedString>(margs[i].selectstat).toString().toStdString();
            } else {
                return std::get<QString>(margs[i].selectstat).toStdString();
            }
        }
    }
    qDebug() << "no proper select statement found :(";
    return "";
}
