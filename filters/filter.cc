/*
    This file is part of Kig, a KDE program for Interactive Geometry.
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filter.h"

#include "cabri-filter.h"
#include "drgeo-filter.h"
#include "kgeo-filter.h"
#include "kseg-filter.h"
#include "native-filter.h"
#include "geogebra-filter.h"

#include <KLocalizedString>
#include <KMessageBox>

KigFilters *KigFilters::sThis;

KigFilter *KigFilters::find(const QString &mime)
{
    for (vect::iterator i = mFilters.begin(); i != mFilters.end(); ++i) {
        if ((*i)->supportMime(mime))
            return *i;
    };
    return nullptr;
}

KigFilters::KigFilters()
{
    mFilters.push_back(KigFilterKGeo::instance());
    mFilters.push_back(KigFilterKSeg::instance());
    mFilters.push_back(KigFilterCabri::instance());
    mFilters.push_back(KigFilterNative::instance());
    mFilters.push_back(KigFilterDrgeo::instance());
    mFilters.push_back(KigFilterGeogebra::instance());
}

KigFilters *KigFilters::instance()
{
    return sThis ? sThis : (sThis = new KigFilters());
}

KigFilter::KigFilter()
{
}

KigFilter::~KigFilter()
{
}

bool KigFilter::supportMime(const QString &)
{
    return false;
}

void KigFilter::fileNotFound(const QString &file) const
{
    KMessageBox::error(nullptr,
                       i18n("The file \"%1\" could not be opened.  "
                            "This probably means that it does not "
                            "exist, or that it cannot be opened due to "
                            "its permissions",
                            file));
}

void KigFilter::parseError(const QString &explanation) const
{
    const QString text = i18n(
        "An error was encountered while parsing this file.  It "
        "cannot be opened.");
    const QString title = i18n("Parse Error");

    if (explanation.isEmpty())
        KMessageBox::error(nullptr, text, title);
    else
        KMessageBox::detailedError(nullptr, text, explanation, title);
}

void KigFilter::notSupported(const QString &explanation) const
{
    KMessageBox::detailedError(nullptr, i18n("Kig cannot open this file."), explanation, i18n("Not Supported"));
}

void KigFilter::warning(const QString &explanation) const
{
    KMessageBox::information(nullptr, explanation);
}

bool KigFilters::save(const KigDocument &data, const QString &tofile)
{
    return KigFilterNative::instance()->save(data, tofile);
}
