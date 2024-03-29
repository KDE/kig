/*
    GeogebraTransformer class for Kig
    SPDX-FileCopyrightText: 2014 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractXmlReceiver>
#include <QMap>
#include <QSet>
#include <QXmlNamePool>

#include <vector>

#include "../misc/point_style.h"
#include "geogebrasection.h"

class KigDocument;
class ObjectHolder;
class ObjectType;

/* This class 'transforms' the XML representation of the GeoGebra file into Kig's
 * internal representation of objects ( with proper parent-child relationship ).
 */
class GeogebraTransformer : public QAbstractXmlReceiver
{
public:
    GeogebraTransformer(KigDocument *document, const QXmlNamePool &np)
        : m_document(document)
        , m_currentState(GeogebraTransformer::ReadingObject)
        , m_currentObject(nullptr)
        , m_nsections(0)
        , m_np(np)
    {
    }
    ~GeogebraTransformer()
    {
    }

    size_t getNumberOfSections() const
    {
        return m_nsections;
    };
    const GeogebraSection &getSection(size_t sectionIdx) const
    {
        return m_sections[sectionIdx];
    };

    // QAbstractXmlReceiver implementation
    void atomicValue(const QVariant &) override;
    void attribute(const QXmlName &name, const QStringRef &value) override;
    void characters(const QStringRef &) override;
    void comment(const QString &) override;
    void endDocument() override;
    void endElement() override;
    void endOfSequence() override;
    void namespaceBinding(const QXmlName &) override;
    void processingInstruction(const QXmlName &, const QString &) override;
    void startDocument() override;
    void startElement(const QXmlName &name) override;
    void startOfSequence() override;

private:
    void resetDrawerVars()
    {
        m_show = true;
        m_thickness = -1;
        m_pointType = Kig::Round;
        m_type = Qt::SolidLine;
        m_r = 0;
        m_g = 0;
        m_b = 0;
        m_alpha = 0;
    };

private:
    enum State {
        ReadingDouble,
        ReadingObject,
        ReadingArguments,
    };

    // Enumerations of the Line Styles used by Geogebra
    // The values 0, 10, 15, 20 are the values used by Geogebra to represent the corresponding styles.
    enum {
        SOLIDLINE = 0,
        DASHDOTDOTLINE = 10,
        DASHLINE = 15,
        DOTLINE = 20,
        DASHDOTLINE = 30,
    };

    // Enumerations of the point styles used by Geogebra.
    enum {
        SOLIDCIRCLEPOINT = 0,
        CROSSPOINT,
        HOLLOWCIRCLEPOINT,
        PLUSPOINT,
        SOLIDDIAMONDPOINT,
        HOLLOWDIAMONDPOINT,
        UPARROWPOINT,
        DOWNARROWPOINT,
        RIGHTARROWPOINT,
        LEFTARROWPOINT
    };

    KigDocument *m_document;
    State m_currentState;
    QMap<QByteArray, ObjectCalcer *> m_objectMap;
    std::vector<ObjectCalcer *> m_currentArgStack;
    const ObjectType *m_currentObject;
    QSet<QByteArray> m_inputObjectLabels;
    QSet<QByteArray> m_outputObjectLabels;
    QByteArray m_currentObjectLabel;
    std::vector<GeogebraSection> m_sections;
    size_t m_nsections;
    QXmlNamePool m_np;
    /* members required for constructing the object-drawers*/
    bool m_show;
    int m_thickness;
    Kig::PointStyle m_pointType;
    Qt::PenStyle m_type;
    int m_r, m_g, m_b, m_alpha; // m_alpha is causing trouble at the moment as Geogebra somehow generates decimal values for it
};
