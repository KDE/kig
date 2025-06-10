/*
    GeogebraTransformer class for Kig
    SPDX-FileCopyrightText: 2014 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "geogebratransformer.h"

#include <kig/kig_document.h>
#include <misc/point_style.h>
#include <objects/bogus_imp.h>
#include <objects/object_calcer.h>
#include <objects/object_drawer.h>
#include <objects/object_factory.h>
#include <objects/object_holder.h>
#include <objects/object_type_factory.h>

#include <libxml/parser.h>
#include <libxslt/transform.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/xsltutils.h>

#include <QDebug>

#include <QColor>
#include <QFile>
#include <QXmlStreamReader>

struct XmlDocDeleter {
    void operator()(xmlDocPtr xmlDoc)
    {
        if (xmlDoc) {
            xmlFreeDoc(xmlDoc);
        }
    }
};

struct XsltStylesheetDeleter {
    void operator()(xsltStylesheetPtr style) const
    {
        if (style) {
            xsltFreeStylesheet(style);
        }
    }
};

using XmlDocPtr = std::unique_ptr<_xmlDoc, XmlDocDeleter>;
using XsltStylesheetPtr = std::unique_ptr<xsltStylesheet, XsltStylesheetDeleter>;
using namespace Qt::StringLiterals;

GeogebraTransformer::GeogebraTransformer(KigDocument *document, const char *xmlContent)
    : m_document(document)
    , m_currentState(GeogebraTransformer::ReadingObject)
    , m_currentObject(nullptr)
    , m_nsections(0)
{
    XmlDocPtr xmlDoc(xmlReadMemory(xmlContent, strlen(xmlContent), "geogebra.xml", nullptr, 0));
    if (!xmlDoc) {
        qWarning() << "Failed to parse XML";
        return;
    }

    QFile queryDevice(QStringLiteral(":/kig/geogebra/geogebra.xsl"));
    const auto ok = queryDevice.open(QFile::ReadOnly);
    if (!ok) {
        qWarning() << "Failed to open geogebra.xsl";
        Q_ASSERT(false);
        return;
    }

    const auto data = queryDevice.readAll();
    qWarning() << data;
    const auto xsltContent = data.constData();
    queryDevice.close();

    XmlDocPtr xsltDoc(xmlReadMemory(xsltContent, strlen(xsltContent), "geogebra.xsl", nullptr, 0));
    if (!xsltDoc) {
        qWarning() << "Failed to parse XSLT document";
        return;
    }

    XsltStylesheetPtr style(xsltParseStylesheetDoc(xsltDoc.release())); // passing the ownership to the style
    if (!style) {
        qWarning() << "Failed to compile XSLT stylesheet";
        return;
    }

    XmlDocPtr result(xsltApplyStylesheet(style.get(), xmlDoc.get(), nullptr));

    if (result) {
        qWarning() << "Failed to apply XSLT stylesheet";
        return;
    }

    xmlChar *docTextPtr = nullptr;
    int docTextLength = 0;
    if (!xsltSaveResultToString(&docTextPtr, &docTextLength, result.get(), style.get())) {
        qWarning() << "Failed to save transformed document";
        return;
    }

    const QByteArray transformedXml(reinterpret_cast<const char *>(docTextPtr), docTextLength);

    xsltCleanupGlobals();
    xmlCleanupParser();

    QXmlStreamReader reader(transformedXml);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            startElement(reader.namespaceUri(), reader.text());
            const auto attributes = reader.attributes();
            for (const auto &attrib : attributes) {
                attribute(attrib.namespaceUri(), attrib.name(), attrib.value());
            }
        } else if (reader.isEndElement()) {
            endElement(reader.namespaceUri(), reader.text());
        }
    }
    if (reader.hasError()) {
        qWarning() << "Failed to read transformed document";
        return;
    }

    m_isValid = true;
}

void GeogebraTransformer::attribute(QStringView namespaceUri, QStringView name, QStringView value)
{
    if (name == "label"_L1) {
        const QByteArray objectLabel = value.toLatin1();
        bool isDoubleValue;
        const double dblval = value.toString().toDouble(&isDoubleValue);

        switch (m_currentState) {
        case GeogebraTransformer::ReadingObject:
            m_currentObjectLabel = value.toLatin1();
            // There must be a better place for this
            m_currentState = GeogebraTransformer::ReadingArguments;

            break;
        case GeogebraTransformer::ReadingArguments:
            if (isDoubleValue) {
                /* This is to handle the circle-point-radius, dilate (and similar) type of Geogebra objects.
                 * <command name="Circle">
                 * <input a0="A" a1="3"/>
                 * <output a0="c"/>
                 *
                 * Notice the attribute 'a1' of the 'input' element. The value - '3' is the radius of the circle.
                 * First, we try to convert that value to Double. If the conversion suceeds, we stack a DoubleImp (Calcer)
                 * in the m_currentArgStack and break. Otherwise, we check the m_objectMap for that label entry.
                 */
                DoubleImp *doubleImp = new DoubleImp(dblval);
                m_currentArgStack.push_back(new ObjectConstCalcer(doubleImp));
            } else if (m_objectMap.contains(objectLabel)) {
                m_currentArgStack.push_back(m_objectMap[objectLabel]);
            } else {
                // TODO Figure out error reporting
            }

            break;
        default:
            break;
        }
    } else if (name == "value"_L1) {
        Q_ASSERT(m_currentState == ReadingDouble);
        DoubleImp *doubleImp = new DoubleImp(value.toString().toDouble());

        m_currentArgStack.push_back(new ObjectConstCalcer(doubleImp));
    } else if (name == "Name"_L1) {
        m_sections[m_nsections - 1].setName(value.toString());
    } else if (name == "Description"_L1) {
        m_sections[m_nsections - 1].setDescription(value.toString());
    } else if (name == "Input"_L1) {
        m_inputObjectLabels.insert(value.toLatin1());
    } else if (name == "Output"_L1) {
        m_outputObjectLabels.insert(value.toLatin1());
    } else if (name == "show"_L1) {
        m_show = ((value.toString() == "true"_L1) ? true : false);
    } else if (name == "thickness"_L1) {
        m_thickness = value.toString().toInt();
    } else if (name == "thickness_point"_L1) {
        m_thickness = value.toString().toInt();
        m_thickness += 6;
    } else if (name == "type"_L1) {
        int penType = value.toString().toInt();
        switch (penType) {
        case SOLIDLINE:
            m_type = Qt::SolidLine;
            break;
        case DASHDOTDOTLINE:
            m_type = Qt::DashDotDotLine;
            break;
        case DASHLINE:
            m_type = Qt::DashLine;
            break;
        case DOTLINE:
            m_type = Qt::DotLine;
            break;
        case DASHDOTLINE:
            m_type = Qt::DashDotLine;
            break;
        default:
            m_type = Qt::SolidLine;
        };
    } else if (name == "pointType"_L1) {
        int pt = value.toString().toInt();
        if (pt == SOLIDCIRCLEPOINT)
            m_pointType = Kig::pointStyleFromString(QStringLiteral("Round"));
        else if (pt == SOLIDDIAMONDPOINT || pt == UPARROWPOINT || pt == DOWNARROWPOINT || pt == RIGHTARROWPOINT || pt == LEFTARROWPOINT)
            m_pointType = Kig::pointStyleFromString(QStringLiteral("Rectangular"));
        else if (pt == HOLLOWCIRCLEPOINT)
            m_pointType = Kig::pointStyleFromString(
                QStringLiteral("Round")); // TODO should be mapped to RoundEmpty ( i.e. 1) but for some reason it is not drawing in KIG
        else if (pt == HOLLOWDIAMONDPOINT)
            m_pointType = Kig::pointStyleFromString(
                QStringLiteral("Rectangular")); // TODO should be mapped to RectangularEmpty ( i.e. 3) but for some reason it is not drawing in KIG
        else if (pt == CROSSPOINT || pt == PLUSPOINT)
            m_pointType = Kig::pointStyleFromString(QStringLiteral("Cross"));
        else
            m_pointType = Kig::Round;
    } else if (name == "r"_L1) {
        m_r = value.toString().toInt();
    } else if (name == "g"_L1) {
        m_g = value.toString().toInt();
    } else if (name == "b"_L1) {
        m_b = value.toString().toInt();
    } else if (name == "alpha"_L1) {
        m_alpha = value.toString().toInt();
    } else if (name == "axes"_L1) {
        bool showAxes = value.toString() == "true"_L1 ? true : false;
        m_document->setAxes(showAxes);
    } else if (name == "grid"_L1) {
        bool showGrid = value.toString() == "true"_L1 ? true : false;
        m_document->setGrid(showGrid);
    }
}

void GeogebraTransformer::endElement(QStringView namespaceUri, QStringView name)
{
    switch (m_currentState) {
    case GeogebraTransformer::ReadingObject:
        if (m_currentObject) {
            if (!m_objectMap.contains(m_currentObjectLabel)) {
                ObjectTypeCalcer *oc = new ObjectTypeCalcer(m_currentObject, m_currentArgStack);

                oc->calc(*m_document);
                m_objectMap.insert(m_currentObjectLabel, oc);

                // Decide where to put this object
                if (m_inputObjectLabels.empty()) {
                    // Not handling input/output objects, put everything in second
                    ObjectDrawer *od = new ObjectDrawer(QColor(m_r, m_g, m_b), m_thickness, m_show, m_type, m_pointType);

                    m_sections[m_nsections - 1].addOutputObject(oc);
                    m_sections[m_nsections - 1].addDrawer(od);
                } else {
                    if (m_inputObjectLabels.contains(m_currentObjectLabel)) {
                        m_sections[m_nsections - 1].addInputObject(oc);
                    } else if (m_outputObjectLabels.contains(m_currentObjectLabel)) {
                        m_sections[m_nsections - 1].addOutputObject(oc);
                    }
                }
            }

            m_currentArgStack.clear();
            m_currentObject = nullptr;
        }

        break;
    case GeogebraTransformer::ReadingArguments:
    case GeogebraTransformer::ReadingDouble:
        /* We do this because every argument may be the
         * last argument of the stack. If it is not, then
         * startElement() will reset this to ReadingArguments
         */
        m_currentState = GeogebraTransformer::ReadingObject;

        break;
    default:
        break;
    }
}

void GeogebraTransformer::startElement(QStringView namespaceUri, QStringView name)
{
    if (name == "Section"_L1) {
        m_nsections++;
        m_sections.push_back(GeogebraSection());

        // Clear stacks
        m_inputObjectLabels.clear();
        m_outputObjectLabels.clear();
        m_currentArgStack.clear();
        m_objectMap.clear();
        return;
    }

    switch (m_currentState) {
    case GeogebraTransformer::ReadingObject:
        if (m_currentObject) {
            // We are already building an object
            m_currentState = GeogebraTransformer::ReadingArguments;
            startElement(namespaceUri, name);
            return;
        }

        {
            resetDrawerVars();
            const QByteArray nameData = name.toLatin1();
            m_currentObject = ObjectTypeFactory::instance()->find(nameData);

            if (!m_currentObject) {
                qWarning() << name << " object not found!";
            }
        }

        break;
    case GeogebraTransformer::ReadingArguments:
        if (name == "Double"_L1) {
            m_currentState = GeogebraTransformer::ReadingDouble;
        }

        break;
    default:
        break;
    }
}
