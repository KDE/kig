// This file is part of Kig, a KDE program for Interactive Geometry.
// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>
// SPDX-FileCopyrightText: 2006 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "cabri-utils.h"

#include "cabri-filter.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpressionMatch>

#include <KLocalizedString>

#define KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR                                                                                                            \
    {                                                                                                                                                          \
        QString locs = i18n("An error was encountered at line %1 in file %2.", __LINE__, __FILE__);                                                            \
        parseError(locs);                                                                                                                                      \
        return nullptr;                                                                                                                                        \
    }

#define KIG_CABRI_FILTER_PARSE_ERROR                                                                                                                           \
    {                                                                                                                                                          \
        QString locs = i18n("An error was encountered at line %1 in file %2.", __LINE__, __FILE__);                                                            \
        parseError(locs);                                                                                                                                      \
        return 0;                                                                                                                                              \
    }

static std::map<QString, QColor> colormap;
static std::map<QString, QColor> colormap_v12;
typedef std::map<QString, QColor>::iterator cmit;

/**
 * Gives the default color for an object, in case of an object come
 * with no color specified; it's exactly what Cabri 1.2 does.
 */
static QColor defaultColorForObject(const QByteArray &type)
{
    if (type == "Line" || type == "Ray" || type == "Seg")
        return Qt::green;
    if (type == "Pt" || type == "Pt/" || type == "Locus")
        return Qt::red;
    if (type == "Cir")
        return Qt::blue;
    if (type == "Pol")
        return QColor(254, 0, 255);
    if (type == "Arc")
        return Qt::cyan;
    if (type == "Vec")
        return QColor(0, 0, 128);
    if (type == "Text" || type == "Angle")
        return Qt::black;

    return Qt::blue;
}

static bool extractValuesFromString(const QString &str, std::vector<int> &vec)
{
    if (!str.isEmpty()) {
        QString tmp = str;
        bool ok = true;
        // extracting the digits
        const QRegularExpression ids("\\d+");
        QRegularExpressionMatch ma(ids.match(tmp));
        while (ma.hasMatch()) {
            vec.push_back(ma.captured(0).toInt(&ok));
            if (!ok)
                return false;
            tmp.remove(ma.capturedStart(), ma.capturedLength());
            ma = ids.match(tmp);
        }
    }
    return true;
}

namespace CabriNS
{
QString readLine(QFile &file)
{
    QString ret = file.readLine(10000L);
    if (!ret.isEmpty() && ret[ret.length() - 1] == '\n')
        ret.truncate(ret.length() - 1);
    if (!ret.isEmpty() && ret[ret.length() - 1] == '\r')
        ret.truncate(ret.length() - 1);
    return ret;
}

QString readText(QFile &f, const QString &s, const QString &sep)
{
    QString line = s;
    if (!line.startsWith('\"') || f.atEnd())
        // don't blame on failing here
        return QString();

    QString tmp = s;
    QString text = tmp;
    while (tmp.at(tmp.length() - 1) != '"') {
        tmp = readLine(f);
        text += sep + tmp;
    }
    QString ret = text.mid(1, text.length() - 2);

    qDebug() << "+++++++++ text: \"" << ret << "\"";

    return ret;
}

} // CabriNS

CabriObject::CabriObject()
    : id(0)
    , thick(1)
    , lineSegLength(0)
    , lineSegSplit(0)
    , visible(true)
    , intersectionId(0)
    , ticks(0)
    , side(0)
    , textRect(Rect::invalidRect())
    , gonio(CabriNS::CG_Deg)
{
}

CabriObject_v10::CabriObject_v10()
    : CabriObject()
    , specialAppearanceSwitch(0)
    , fixed(false)
{
}

CabriObject_v12::CabriObject_v12()
    : CabriObject()
    , pointStyle(1)
{
}

CabriReader::CabriReader(const KigFilterCabri *filter)
    : m_filter(filter)
{
    initColorMap();
}

CabriReader::~CabriReader()
{
}

void CabriReader::initColorMap()
{
    static bool colors_initialized = false;
    if (!colors_initialized) {
        colors_initialized = true;
        colormap[QStringLiteral("R")] = Qt::red;
        colormap[QStringLiteral("O")] = Qt::magenta;
        colormap[QStringLiteral("Y")] = Qt::yellow;
        colormap[QStringLiteral("P")] = Qt::darkMagenta;
        colormap[QStringLiteral("V")] = Qt::darkBlue;
        colormap[QStringLiteral("Bl")] = Qt::blue;
        colormap[QStringLiteral("lBl")] = Qt::cyan;
        colormap[QStringLiteral("G")] = Qt::green;
        colormap[QStringLiteral("dG")] = Qt::darkGreen;
        colormap[QStringLiteral("Br")] = QColor(165, 42, 42);
        colormap[QStringLiteral("dBr")] = QColor(128, 128, 0);
        colormap[QStringLiteral("lGr")] = Qt::lightGray;
        colormap[QStringLiteral("Gr")] = Qt::gray;
        colormap[QStringLiteral("dGr")] = Qt::darkGray;
        colormap[QStringLiteral("B")] = Qt::black;
        colormap[QStringLiteral("W")] = Qt::white;
    }
}

void CabriReader::parseError(const QString &explanation) const
{
    m_filter->parseError(explanation);
}

QColor CabriReader::translateColor(const QString &s)
{
    initColorMap();
    cmit it = colormap.find(s);
    if (it != colormap.end())
        return (*it).second;

    qDebug() << "unknown color: " << s;
    return Qt::black;
}

CabriReader_v10::CabriReader_v10(const KigFilterCabri *filter)
    : CabriReader(filter)
{
}

CabriReader_v10::~CabriReader_v10()
{
}

bool CabriReader_v10::readWindowMetrics(QFile &f)
{

    QString line = CabriNS::readLine(f);
    const QRegularExpression first("^Window center x: (.+) y: (.+) Window size x: (.+) y: (.+)$");
    if (!first.match(line).hasMatch())
        KIG_CABRI_FILTER_PARSE_ERROR;

    line = CabriNS::readLine(f);

    return true;
}

CabriObject *CabriReader_v10::readObject(QFile &f)
{
    CabriObject_v10 *myobj = new CabriObject_v10();


    bool ok;
    QString tmp;

    QString line1 = CabriNS::readLine(f);
    const QRegularExpression namelinere("^\"([^\"]+)\", NP: ([\\d-]+), ([\\d-]+), NS: (\\d+), (\\d+)$");
    const QRegularExpressionMatch namelinema(namelinere.match(line1));
    if (namelinema.hasMatch()) {
        tmp = namelinema.captured(1);
        myobj->name = tmp;

        line1 = CabriNS::readLine(f);
    }

    const QRegularExpression firstlinere("^([^:]+): ([^,]+), ([^,]+), CN:([^,]*), VN:(.*)$");
    const QRegularExpressionMatch firstlinema(firstlinere.match(line1));
    if (!firstlinema.hasMatch())
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    tmp = firstlinema.captured(1);
    myobj->id = tmp.toUInt(&ok);
    if (!ok)
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    tmp = firstlinema.captured(2);
    myobj->type = tmp.toLatin1();

    tmp = firstlinema.captured(3);
    myobj->specification = tmp.toInt(&ok);
    if (!ok)
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
    // for "Eq/Co" this is presumably what we want:
    //  0: x coordinate (of point)
    //  1: y coordinate (of point)

    tmp = firstlinema.captured(4);
    uint numberOfParents = tmp.toUInt(&ok);
    if (!ok)
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    tmp = firstlinema.captured(5);
    // I have no idea what this number means.

    QString line2 = CabriNS::readLine(f);
    const QRegularExpression secondlinere("^([^,]+), ([^,]+), ([^,]+), DS:([^ ]+) ([^,]+), GT:([^,]+), ([^,]+), (.*)$");
    const QRegularExpression secondlinere2("^([^,]+), ([^,]+), NbD:([^,]+), ([^,]+), ([^,]+), GT:([^,]+), ([^,]+), (.*)$");
    const QRegularExpressionMatch secondlinema(secondlinere.match(line2));
    const QRegularExpressionMatch secondlinema2(secondlinere2.match(line2)); // e.g. for AngVal
    if (secondlinema.hasMatch()) {
        tmp = secondlinema.captured(1);
        myobj->color = translateColor(tmp);

        tmp = secondlinema.captured(2);
        myobj->fillColor = translateColor(tmp);

        tmp = secondlinema.captured(3);
        myobj->thick = tmp == QLatin1String("t") ? 1 : tmp == QLatin1String("tT") ? 2 : 3;

        tmp = secondlinema.captured(4);
        myobj->lineSegLength = tmp.toInt(&ok);
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

        tmp = secondlinema.captured(5);
        myobj->lineSegSplit = tmp.toInt(&ok);
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

        tmp = secondlinema.captured(6);
        myobj->specialAppearanceSwitch = tmp.toInt(&ok);
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

        tmp = secondlinema.captured(7);
        myobj->visible = tmp == QLatin1String("V");

        tmp = secondlinema.captured(8);
        myobj->fixed = tmp == QLatin1String("St");
    } else if (secondlinema2.hasMatch())
    {
        tmp = secondlinema2.captured(1);
        myobj->color = translateColor(tmp);

        tmp = secondlinema2.captured(2);
        myobj->fillColor = translateColor(tmp);

        // 3: e.g. "NbD:129" what is the meaning?
        // 4: e.g. "FD"

        tmp = secondlinema2.captured(5);
        if (tmp == QLatin1String("deg"))
            myobj->gonio = CabriNS::CG_Deg;

        tmp = secondlinema2.captured(6);
        myobj->specialAppearanceSwitch = tmp.toInt(&ok);
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

        tmp = secondlinema2.captured(7);
        myobj->visible = tmp == QLatin1String("V");

        tmp = secondlinema2.captured(8);
        myobj->fixed = tmp == QLatin1String("St");

    } else
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    QString line3 = CabriNS::readLine(f);
    const QRegularExpression thirdlinere("^(Const: ([^,]*),? ?)?(Val: ([^,]*)(,(.*))?)?$");
    const QRegularExpressionMatch thirdlinema(thirdlinere.match(line3));
    if (!thirdlinema.hasMatch())
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    tmp = thirdlinema.captured(2);
    const QStringList parentsids = tmp.split(' ', Qt::SkipEmptyParts);
    for (QStringList::const_iterator i = parentsids.begin(); i != parentsids.end(); ++i) {
        myobj->parents.push_back((*i).toInt(&ok));
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
    }
    if (myobj->parents.size() != numberOfParents)
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    tmp = thirdlinema.captured(4);
    const QStringList valIds = tmp.split(' ', Qt::SkipEmptyParts);
    for (QStringList::const_iterator i = valIds.begin(); i != valIds.end(); ++i) {
        myobj->data.push_back((*i).toDouble(&ok));
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
    }

    const QString thirdlineextra = thirdlinema.captured(6);
    if (myobj->type == "Text" || myobj->type == "Formula") {
        const QRegularExpression textMetrics("TP: *[\\s]*([^,]+), *[\\s]*([^,]+), *TS:[\\s]*([^,]+), *[\\s]*([^,]+)");
        const QRegularExpressionMatch textMetricsMa(textMetrics.match(thirdlineextra));
        if (textMetricsMa.hasMatch()) {
            double xa = textMetricsMa.captured(1).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            double ya = textMetricsMa.captured(2).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            double tw = textMetricsMa.captured(3).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            double th = textMetricsMa.captured(4).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            myobj->textRect = Rect(xa, ya, tw, th);
        }
        QString textline = CabriNS::readLine(f);
        if (textline.isEmpty())
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
        if (myobj->type == "Formula") {
            // Just hope there is no escaping!
            myobj->text = textline;
        } else
            myobj->text = CabriNS::readText(f, textline, QStringLiteral("\n"));

        // Hack: extracting substitution arguments from the parents list
        // and inserting them into the "incs" vector
        // under the assumption that the required parents are the last
        // in the arguments list (in forward order)
        // note that we do not check for special escaping of the symbols
        // '"' and '#'
        int count = myobj->text.count(QStringLiteral("\"#"));
        int parentsnum = myobj->parents.size();
        for (int i = parentsnum - count; i < parentsnum; ++i)
            myobj->incs.push_back(myobj->parents[i]);
    }

    QString line4 = CabriNS::readLine(f);
    if (!line4.isEmpty()) {
        const QRegularExpression fontlinere("^p: (\\d+), ([^,]+), S: (\\d+) C: (\\d+) Fa: (\\d+)$");
        const QRegularExpressionMatch fontlinema(fontlinere.match(line4));
        if (!fontlinema.hasMatch())
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

        line4 = CabriNS::readLine(f);
    }

    return myobj;
}

void CabriReader_v10::decodeStyle(CabriObject *obj, Qt::PenStyle &ps, Kig::PointStyle &pointType)
{
    CabriObject_v10 *myobj = (CabriObject_v10 *)obj;

    if ((myobj->type == "Pt") || (myobj->type == "Pt/")) {
        switch (myobj->specialAppearanceSwitch) {
        case 0: {
            myobj->thick -= 1;
            break;
        }
        case 2: {
            myobj->thick += 1;
            break;
        }
        case 3: {
            myobj->thick += 1;
            pointType = Kig::RoundEmpty;
            break;
        }
        case 4: {
            myobj->thick += 2;
            pointType = Kig::Cross;
            break;
        }
        }
        myobj->thick *= 2;
    } else {
        if ((myobj->lineSegLength > 1) && (myobj->lineSegLength < 6) && (myobj->lineSegSplit > 1) && (myobj->lineSegSplit <= 10))
            ps = Qt::DotLine;
        else if ((myobj->lineSegLength >= 6) && (myobj->lineSegSplit > 10))
            ps = Qt::DashLine;
    }
}

CabriReader_v12::CabriReader_v12(const KigFilterCabri *filter)
    : CabriReader(filter)
{
    initColorMap();
}

CabriReader_v12::~CabriReader_v12()
{
}

void CabriReader_v12::initColorMap()
{
    static bool colors_initialized = false;
    if (!colors_initialized) {
        colors_initialized = true;
        CabriReader::initColorMap();
        colormap_v12 = colormap;
        colormap_v12[QStringLiteral("dkg")] = QColor(0, 100, 0);
        colormap_v12[QStringLiteral("old")] = QColor(107, 142, 35);
        colormap_v12[QStringLiteral("olv")] = QColor(128, 128, 0);
        colormap_v12[QStringLiteral("lig")] = QColor(50, 205, 50);
        colormap_v12[QStringLiteral("gry")] = QColor(173, 255, 47);
        colormap_v12[QStringLiteral("gor")] = QColor(218, 165, 32);
        colormap_v12[QStringLiteral("msg")] = QColor(0, 250, 154);
        colormap_v12[QStringLiteral("spg")] = QColor(0, 255, 127);
        colormap_v12[QStringLiteral("pag")] = QColor(152, 251, 152);
        colormap_v12[QStringLiteral("kki")] = QColor(240, 230, 140);
        colormap_v12[QStringLiteral("O")] = QColor(255, 140, 0);
        colormap_v12[QStringLiteral("Br")] = QColor(165, 42, 42);
        colormap_v12[QStringLiteral("tea")] = QColor(0, 128, 128);
        colormap_v12[QStringLiteral("pat")] = QColor(175, 238, 238);
        colormap_v12[QStringLiteral("ltp")] = QColor(255, 182, 193);
        colormap_v12[QStringLiteral("dBr")] = QColor(128, 0, 0);
        colormap_v12[QStringLiteral("lsg")] = QColor(32, 178, 170);
        colormap_v12[QStringLiteral("dob")] = QColor(30, 144, 255);
        colormap_v12[QStringLiteral("skb")] = QColor(135, 206, 235);
        colormap_v12[QStringLiteral("plm")] = QColor(221, 160, 221);
        colormap_v12[QStringLiteral("dep")] = QColor(255, 20, 147);
        colormap_v12[QStringLiteral("crs")] = QColor(220, 20, 60);
        colormap_v12[QStringLiteral("rob")] = QColor(65, 105, 225);
        colormap_v12[QStringLiteral("blv")] = QColor(138, 43, 226);
        colormap_v12[QStringLiteral("ma")] = QColor(254, 0, 255);
        colormap_v12[QStringLiteral("mvr")] = QColor(199, 21, 133);
        colormap_v12[QStringLiteral("ind")] = QColor(75, 0, 130);
        colormap_v12[QStringLiteral("meo")] = QColor(186, 85, 211);
        colormap_v12[QStringLiteral("Gr")] = QColor(128, 128, 128);
        colormap_v12[QStringLiteral("dGr")] = QColor(64, 64, 64);
    }
}

bool CabriReader_v12::readWindowMetrics(QFile &f)
{

    QString line = CabriNS::readLine(f);
    const QRegularExpression first("^Window center x: (.+) y: (.+) Window size x: (.+) y: (.+)$");
    if (!first.match(line).hasMatch())
        KIG_CABRI_FILTER_PARSE_ERROR;

    QString line2 = CabriNS::readLine(f);
    const QRegularExpression second("^Resolution: (\\d+) ppc$");
    if (!second.match(line2).hasMatch())
        KIG_CABRI_FILTER_PARSE_ERROR;

    line = CabriNS::readLine(f);

    return true;
}

CabriObject *CabriReader_v12::readObject(QFile &f)
{
    CabriObject_v12 *myobj = new CabriObject_v12();

    QString line = CabriNS::readLine(f);

#ifdef CABRI_DEBUG
    qDebug() << "+++++++++ line: \"" << line << "\"";
#endif
    const QRegularExpression firstlinere(
        "^([^:]+): ([^,]+), (Const: [,0-9\\s]+)?(int ind:([^,]+),\\s)?(cart, )?(side:(\\d+), )?(inc\\.elmts: ([,0-9\\s]+))?(axis:(x|y), )?(on mark, )?(Val: "
        "([^,]+))?(.*)$");
    const QRegularExpressionMatch firstlinema(firstlinere.match(line));
    if (!firstlinema.hasMatch())
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    bool ok = true;
    QString tmp;

    // id
    tmp = firstlinema.captured(1);
    myobj->id = tmp.toUInt(&ok);
    if (!ok)
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    // type
    tmp = firstlinema.captured(2);
    myobj->type = tmp.toLatin1();

    // parents
    tmp = firstlinema.captured(3);
    if (!extractValuesFromString(tmp, myobj->parents))
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    // data
    tmp = firstlinema.captured(15);
    const QStringList valIds = tmp.split(' ', Qt::SkipEmptyParts);
    for (QStringList::const_iterator i = valIds.begin(); i != valIds.end(); ++i) {
        myobj->data.push_back((*i).toDouble(&ok));
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
    }

    // intersection id (if present)
    tmp = firstlinema.captured(5);
    if (!tmp.isEmpty()) {
        long intId = tmp.toLong(&ok, 16);
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
#ifdef CABRI_DEBUG
        qDebug() << "+++++++++ intId: " << intId;
#endif
        if (intId == 0)
            myobj->intersectionId = -1;
        else if (intId == 0x10000)
            myobj->intersectionId = 1;
        else
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
    }

    // side of a polygon (if present)
    tmp = firstlinema.captured(8);
    if (!tmp.isEmpty()) {
        myobj->side = tmp.toInt(&ok);
        if (!ok)
            KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
    }

    // inc.elements (if present)
    tmp = firstlinema.captured(10);
    if (!extractValuesFromString(tmp, myobj->incs))
        KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;

    line = CabriNS::readLine(f);

    const QRegularExpression textMetrics("^TP:[\\s]*([^,]+),[\\s]*([^,]+), TS:[\\s]*([^,]+),[\\s]*([^,]+)$");
    const QRegularExpressionMatch textMetricsMa(textMetrics.match(line));
    bool freeText = false;
    while (!line.isEmpty()) {
        if (line.startsWith('\"')) {
            QString txt = CabriNS::readText(f, line);
            if (myobj->type != "Text")
                myobj->name = txt;
            else
                myobj->text = txt;
        } else if (line.startsWith(QLatin1String("NbD:"))) {
            // TODO
        } else if (textMetricsMa.hasMatch()) {
            double xa = textMetricsMa.captured(1).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            double ya = textMetricsMa.captured(2).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            double tw = textMetricsMa.captured(3).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            double th = textMetricsMa.captured(4).toDouble(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR_RETURN_NULLPTR;
            myobj->textRect = Rect(xa, ya, tw, th);
        } else {
            if (!freeText && myobj->type == "Formula") {
                myobj->text = line;
                freeText = true;
            } else
                readStyles(line, myobj);
        }

        line = CabriNS::readLine(f);
    }

    // default color
    if (!myobj->color.isValid())
        myobj->color = defaultColorForObject(myobj->type);

    return myobj;
}

void CabriReader_v12::decodeStyle(CabriObject *obj, Qt::PenStyle &ps, Kig::PointStyle &pointType)
{
    CabriObject_v12 *myobj = (CabriObject_v12 *)obj;

    if ((myobj->type == "Pt") || (myobj->type == "Pt/")) {
        // different sizes for points.
        myobj->thick *= 2;
        switch (myobj->pointStyle) {
        case 0: {
            myobj->thick /= 2;
            break;
        }
        case 1: {
            pointType = Kig::Rectangular;
            break;
        }
        case 2: {
            pointType = Kig::RoundEmpty;
            break;
        }
        case 3: {
            pointType = Kig::Cross;
            break;
        }
        case 4: {
            break;
        }
        }
    } else {
        if ((myobj->lineSegLength > 1) && (myobj->lineSegLength < 6) && (myobj->lineSegSplit > 1) && (myobj->lineSegSplit <= 10))
            ps = Qt::DotLine;
        else if ((myobj->lineSegLength >= 6) && (myobj->lineSegSplit > 10))
            ps = Qt::DashLine;
    }
}

QColor CabriReader_v12::translateColor(const QString &s)
{
    initColorMap();
    cmit it = colormap_v12.find(s);
    if (it != colormap_v12.end())
        return (*it).second;

    qDebug() << "unknown color: " << s;
    return CabriReader::translateColor(s);
}

bool CabriReader_v12::readStyles(const QString &line, CabriObject_v12 *myobj)
{
#ifdef CABRI_DEBUG
    qDebug() << ">>>>>>>>> style line: \"" << line << "\"";
#endif
    QStringList styles = line.split(QStringLiteral(", "), Qt::SkipEmptyParts);
    bool ok = true;
    for (QStringList::iterator it = styles.begin(); it != styles.end(); ++it) {
        if ((*it) == QLatin1String("invisible")) {
            myobj->visible = false;
        } else if ((*it).startsWith(QLatin1String("DS:"))) {
            const QRegularExpression ticks(QRegularExpression::anchoredPattern("DS:(\\d+)\\s(\\d+)"));
            const QRegularExpressionMatch ticksma(ticks.match((*it)));
            if (ticksma.hasMatch()) {
                myobj->lineSegLength = ticksma.captured(1).toInt(&ok);
                if (!ok)
                    KIG_CABRI_FILTER_PARSE_ERROR;
                myobj->lineSegSplit = ticksma.captured(2).toInt(&ok);
                if (!ok)
                    KIG_CABRI_FILTER_PARSE_ERROR;
            }
        }
        // colors
        else if ((*it).startsWith(QLatin1String("color:"))) {
            QString color = (*it).remove(0, 6);
            myobj->color = translateColor(color);
        } else if ((*it).startsWith(QLatin1String("fill color:"))) {
            QString color = (*it).remove(0, 11);
            myobj->fillColor = translateColor(color);
        }
        // styles
        else if ((*it) == QLatin1String("thicker")) {
            myobj->thick = 2;
        } else if ((*it) == QLatin1String("thick")) {
            myobj->thick = 3;
        }
        // point types
        else if ((*it) == QLatin1String("1x1")) {
            myobj->pointStyle = 0;
        } else if ((*it) == QLatin1String("CIRCLE")) {
            myobj->pointStyle = 2;
        } else if ((*it) == QLatin1String("TIMES")) {
            myobj->pointStyle = 3;
        } else if ((*it) == QLatin1String("PLUS")) {
            myobj->pointStyle = 4;
        }
        // goniometry systems
        else if ((*it) == QLatin1String("deg")) {
            myobj->gonio = CabriNS::CG_Deg;
        }
        // object ticks
        else if ((*it).startsWith(QLatin1String("marks nb:"))) {
            QString strticks = (*it).remove(0, 9);
            myobj->ticks = strticks.toInt(&ok);
            if (!ok)
                KIG_CABRI_FILTER_PARSE_ERROR;
        }
#ifdef CABRI_DEBUG
        else {
            qDebug() << ">>>>>>>>> UNKNOWN STYLE STRING: \"" << *it << "\"";
        }
#endif
    }
    return true;
}
