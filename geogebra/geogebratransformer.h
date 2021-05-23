/*
    GeogebraTransformer class for Kig
    SPDX-FileCopyrightText: 2014 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef GEOGEBRATRANSFORMER_H
#define GEOGEBRATRANSFORMER_H

#include <QMap>
#include <QSet>
#include <QAbstractXmlReceiver>
#include <QXmlNamePool>

#include <vector>

#include "geogebrasection.h"
#include "../misc/point_style.h"

class KigDocument;
class ObjectHolder;
class ObjectType;

/* This class 'transforms' the XML representation of the GeoGebra file into Kig's 
 * internal representation of objects ( with proper parent-child relationship ).
 */
class GeogebraTransformer : public QAbstractXmlReceiver
{
public:
    GeogebraTransformer( KigDocument * document, const QXmlNamePool &np ) :
    m_document( document ),
    m_currentState ( GeogebraTransformer::ReadingObject ),
    m_currentObject( nullptr ),
    m_nsections( 0 ),
    m_np( np )
    {}
    ~GeogebraTransformer() {}

    size_t getNumberOfSections() const { return m_nsections; };
    const GeogebraSection & getSection( size_t sectionIdx ) const { return m_sections[sectionIdx]; };

    // QAbstractXmlReceiver implementation
    void atomicValue ( const QVariant & ) Q_DECL_OVERRIDE;
    void attribute ( const QXmlName & name, const QStringRef & value ) Q_DECL_OVERRIDE;
    void characters ( const QStringRef & ) Q_DECL_OVERRIDE;
    void comment ( const QString & ) Q_DECL_OVERRIDE;
    void endDocument() Q_DECL_OVERRIDE;
    void endElement() Q_DECL_OVERRIDE;
    void endOfSequence() Q_DECL_OVERRIDE;
    void namespaceBinding ( const QXmlName & ) Q_DECL_OVERRIDE;
    void processingInstruction ( const QXmlName &, const QString & ) Q_DECL_OVERRIDE;
    void startDocument() Q_DECL_OVERRIDE;
    void startElement ( const QXmlName & name ) Q_DECL_OVERRIDE;
    void startOfSequence() Q_DECL_OVERRIDE;

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

    KigDocument * m_document;
    State m_currentState;
    QMap<QByteArray, ObjectCalcer *> m_objectMap;
    std::vector<ObjectCalcer *> m_currentArgStack;
    const ObjectType * m_currentObject;
    QSet<QByteArray> m_inputObjectLabels;
    QSet<QByteArray> m_outputObjectLabels;
    QByteArray m_currentObjectLabel;
    std::vector<GeogebraSection> m_sections;
    size_t m_nsections;
    QXmlNamePool m_np;
    /* members required for constructing the object-drawers*/
    bool m_show;
    int  m_thickness;
    Kig::PointStyle m_pointType;
    Qt::PenStyle  m_type;
    int  m_r, m_g, m_b, m_alpha; // m_alpha is causing trouble at the moment as Geogebra somehow generates decimal values for it
};

#endif // GEOGEBRATRANSFORMER_H
