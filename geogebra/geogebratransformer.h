/*
 * GeogebraTransformer class for Kig
 * Copyright 2014  David E. Narvaez <david.narvaez@computer.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GEOGEBRATRANSFORMER_H
#define GEOGEBRATRANSFORMER_H

#include <QMap>
#include <QSet>
#include <QAbstractXmlReceiver>
#include <QXmlNamePool>

#include <vector>

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
    GeogebraTransformer( KigDocument * document, QXmlNamePool np ) :
    m_document( document ),
    m_currentState ( GeogebraTransformer::ReadingObject ),
    m_currentObject( nullptr ),
    m_nsections( 0 ),
    m_np( np )
    {}
    ~GeogebraTransformer() {}

    size_t getNumberOfSections() const { return m_nsections; };
    const GeogebraSection getSection( size_t sectionIdx ) const { return m_sections[sectionIdx]; };

    // QAbstractXmlReceiver implementation
    virtual void atomicValue ( const QVariant & );
    virtual void attribute ( const QXmlName & name, const QStringRef & value );
    virtual void characters ( const QStringRef & );
    virtual void comment ( const QString & );
    virtual void endDocument();
    virtual void endElement();
    virtual void endOfSequence();
    virtual void namespaceBinding ( const QXmlName & );
    virtual void processingInstruction ( const QXmlName &, const QString & );
    virtual void startDocument();
    virtual void startElement ( const QXmlName & name );
    virtual void startOfSequence();

private:
    enum State {
        ReadingDouble,
        ReadingObject,
        ReadingArguments,
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
};

#endif // GEOGEBRATRANSFORMER_H
