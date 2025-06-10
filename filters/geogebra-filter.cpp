/*
    GeoGebra Filter for Kig
    SPDX-FileCopyrightText: 2013 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

*/

#include "geogebra-filter.h"
#include "geogebra/geogebratransformer.h"

// #include <geogebra/geogebratransformer.h>
#include <kig/kig_document.h>
#include <objects/bogus_imp.h>
#include <objects/object_calcer.h>
#include <objects/object_factory.h>
#include <objects/object_holder.h>
#include <objects/object_type_factory.h>

#include <KZip>
#include <QDebug>

#include <QFile>
#include <QXmlStreamReader>

#include <algorithm>

KigFilterGeogebra *KigFilterGeogebra::instance()
{
    static KigFilterGeogebra f;
    return &f;
}

bool KigFilterGeogebra::supportMime(const QString &mime)
{
    return mime == QLatin1String("application/vnd.geogebra.file");
}

static ObjectHolder *holderFromCalcerAndDrawer(ObjectCalcer *oc, ObjectDrawer *od)
{
    return new ObjectHolder(oc, od);
}

KigDocument *KigFilterGeogebra::load(const QString &sFrom)
{
    KZip geogebraFile(sFrom);
    KigDocument *document = new KigDocument();

    if (!geogebraFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open zip archive";
        return document;
    }

    const KZipFileEntry *geogebraXMLEntry = dynamic_cast<const KZipFileEntry *>(geogebraFile.directory()->entry(QStringLiteral("geogebra.xml")));

    if (!geogebraXMLEntry) {
        qWarning() << "No geogebra.xml found in the ZIP archive";
        return document;
    }

    GeogebraTransformer transformer(document, geogebraXMLEntry->data().constData());

    Q_ASSERT(transformer.getNumberOfSections() == 1);

    const GeogebraSection &gs = transformer.getSection(0);
    const std::vector<ObjectCalcer *> &f = gs.getOutputObjects();
    const std::vector<ObjectDrawer *> &d = gs.getDrawers();
    std::vector<ObjectHolder *> holders(f.size());

    std::transform(f.cbegin(), f.cend(), d.begin(), holders.begin(), holderFromCalcerAndDrawer);

    document->addObjects(holders);

    return document;
}
